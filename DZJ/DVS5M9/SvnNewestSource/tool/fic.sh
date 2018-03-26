#!/bin/sh
#/**************************************************************************
# *
# *        Copyright (c) 2010 by Sunplus mMedia Inc., Ltd.
# *
# *  This software is copyrighted by and is the property of Sunplus
# *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
# *  Inc., Ltd. This software may only be used in accordance with the
# *  corresponding license agreement. Any unauthorized use, duplication,
# *  distribution, or disclosure of this software is expressly forbidden.
# *
# *  This Copyright notice may not be removed or modified without prior
# *  written consent of Sunplus mMedia Inc., Ltd.
# *
# *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
# *  software without notice.
# *
# *  Sunplus mMedia Inc., Ltd.
# *  19-1, Innovation First Road, Science-Based Industrial Park,
# *  Hsin-Chu, Taiwan.
# *
# *  Author: Anmin Deng
# *
# **************************************************************************/

# to merge files specified by arguments into one FIC (or File In Code) binary
# \param ${1} is the result FIC binary file name
# \param remains are the files to be merged

if [ $# -lt 2 ] ; then
	echo "${0}: usage: \`fic' \`files..'" 1>&2
	exit 1
fi

fic=${1}

s=[[:blank:]]

dirname=/usr/bin/dirname
sed=/bin/sed
sort=/usr/bin/sort
wc=/usr/bin/wc
cut=/usr/bin/cut
rm=/bin/rm
touch=/bin/touch
head=/usr/bin/head
cat=/bin/cat

toolpath=`$dirname ${0}` &&
t2b=${toolpath}/t2b
if [ $? != 0 ] || [ ! -x "$t2b" ] ; then
	echo "${0}:\`${t2b}':invalid" 1>&2
	exit 1
fi

set -o pipefail

shift 1
nfiles=0
sumsize=0
files=`echo $* | $sed -e "s/^$s\+//" -e "s/$s\+/\n/g" | $sort -u` &&
for f in ${files} ; do
	if [ ! -f "${f}" ] || [ ! -r "${f}" ] ; then
		echo "${0}: \`${f}': bin can not open" 1>&2
		exit 1
	fi &&
	if [[ "${f}" != */A/RO_RES/* ]] ; then
		echo "${0}: \`${f}': bin not in A:\\RO_RES\\" 1>&2
		exit 1
	fi &&
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1` &&
	if (( "${fsize}" >= 0x10000000 )) ; then
		echo "${0}: \`${f}': size \`${fsize}' too big" 1>&2
		exit 1
	fi &&
	ff=${f#*/A/RO_RES/} &&
	ff=${ff//\//\\} &&
	fnamelen=${#ff} &&
	if [ ${fnamelen} -ge 24 ] ; then
		echo "${0}: \`${ff}': file name too long" 1>&2
		exit 1
	fi &&
	(( ++nfiles )) &&
	(( sumsize += ( `$wc -c ${f} | $cut -d ' ' -f 1` + 15 ) & -16 ))
	# zero size file may have problems when sumsize==0
	if [ $? != 0 ] ; then
		echo "${0}:\`${f}':file error" 1>&2
		exit 1
	fi
done
if [ $? != 0 ] ; then
	echo "${0}:fic entries error" 1>&2
	exit 1
fi

if [ ${nfiles} = 0 ] ; then
	echo "${0}: no fic" 1>&2
	exit 0
fi

foffset=$(( ${nfiles} * 32 + 16 ))
(( sumsize += ${foffset} ))


# FIC header
# t2b direct to stdout may convert \n to \r\n, workaround to output to file
echo -e "${sumsize}\n${nfiles}\n0\n0" | $t2b -b 4 - ${fic}
if [ $? != 0 ] ; then
	echo "${0}: \`${fic}': fic header error" 1>&2
	exit 1
fi

# FIC entries
for f in ${files} ; do
	ff=${f#*/A/RO_RES/}
	ff=${ff//\//\\}
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1`
	if [ $? != 0 ] ; then
		echo "${0}:\`${f}':file size error" 1>&2
		exit 1
	fi
	poffset=${foffset}
	if [ "${fsize}" -gt 65535 ] ; then
		# seek on large file not supported, set "large" bit
		(( poffset += 0x80000000 ))
	fi
	fnamelen=${#ff}
	fillsize=$(( 24 - ${fnamelen} ))
# t2b direct to stdout may convert \n to \r\n, workaround to output to file
	echo -e "${poffset}\n${fsize}" | $t2b -a -b 4 - ${fic} &&
	echo -n "${ff}" >> ${fic} &&
	$head -c ${fillsize} /dev/zero >> ${fic}
	if [ $? != 0 ] ; then
		echo "${0}: \`${ff}': fic entry error" 1>&2
		exit 1
	fi
	(( foffset += ( ${fsize} + 15 ) & -16 ))
done

# FIC contains
for f in ${files} ; do
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1`
	if [ $? != 0 ] ; then
		echo "${0}:\`${f}':file size error" 1>&2
		exit 1
	fi
	fillsize=$(( ( ( ${fsize} + 15 ) & -16 ) - ${fsize} ))
	$cat ${f} >> ${fic} &&
	$head -c ${fillsize} /dev/zero >> ${fic}
	if [ $? != 0 ] ; then
		echo "${0}: \`${f}': fic contains error" 1>&2
		exit 1
	fi
done
