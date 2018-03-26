#!/bin/sh
#/**************************************************************************
# *
# *        Copyright (c) 2010-2012 by iCatch Technology Inc.
# *
# *  This software is copyrighted by and is the property of iCatch
# *  Technology Inc. All rights are reserved by iCatch Technology Inc.
# *  This software may only be used in accordance with the corresponding
# *  license agreement. Any unauthorized use, duplication, distribution,
# *  or disclosure of this software is expressly forbidden.
# *
# *  This Copyright notice MUST not be removed or modified without prior
# *  written consent of iCatch Technology Inc.
# *
# *  iCatch Technology Inc. reserves the right to modify this software
# *  without notice.
# *
# *  iCatch Technology Inc.
# *  19-1, Innovation First Road, Science-Based Industrial Park,
# *  Hsin-Chu, Taiwan.
# *
# **************************************************************************/
# \file   sym_addr_find.sh
# \brief  to get (including static) symbol address
# from all.sym file which was derived from "objdump -t <elf file>"
# \author Anmin Deng

# \param
# ${1}, the .all.sym file derived from "objdump -t <elf file>";
# ${2}, the symbol name.
# <src>::<key> searching symbol in specific source file;
# <key> searching symbol in all the elf;
# \return
# exit code 0: success; non-zero: failed
# result to print address value, in hex with prefix 0x, to <stdout>

if [ $# != 2 ] ; then
	echo "${0}:\`allsym' [\`src'::]\`symbol'" 1>&2
	exit 1
fi

if [ ! -f "${1}" ] || [ ! -r "${1}" ] ; then
	echo "${0}:\`${1}':allsym file invalid" 1>&2
	exit 1
fi

sym=${1}
fullkey=${2}

x=[[:xdigit:]]
s=[[:blank:]]

# tool paths
#basename=/bin/basename
dirname=/bin/dirname
#cat=/bin/cat
rm=/bin/rm
#cp=/bin/cp
#mv=/bin/mv
#mkdir=/bin/mkdir
#touch=/bin/touch
sed=/bin/sed
#grep=/bin/grep
head=/usr/bin/head
cut=/usr/bin/cut
od=/usr/bin/od
csplit=/usr/bin/csplit

t2b=`$dirname ${0}`/t2b

if [ $? != 0 ] || [ ! -x "${t2b}" ] ; then
	echo "${0}:\`${t2b}':external tools invalid" 1>&2
	exit 1
fi

set -o pipefail

key=${fullkey#*::}
src=${fullkey%${key}}
src=${src%::}
line=

if [ -z "${key}" ] ; then
	echo "${0}:\`${fullkey}':bad <src>::<key>" 1>&2
	exit 1
fi

if [ -z "${src}" ] ; then
	line=`$sed -ne \
	"s/^\($x\{8\}\) .*$s$x\{8\} \(\(0xf0 \)\?${key}\)$/\1 \2/p" ${sym}`
else
	marker="^00000000 l    df \*ABS\*${s}00000000 "

	$rm -f ${sym}00 ${sym}.${src}[0-9]* &&
	$sed -e 's/\r//g' ${sym} | \
	$csplit -q -f ${sym} - "%${marker}${src}$%0" &&
	$csplit -q -z -f ${sym}.${src} ${sym}00 "/${marker}${src}$/0" '{*}' &&
	$rm -f ${sym}00 &&
	for f in ${sym}.${src}[0-9]* ; do
		line=`$sed -e "/${marker}${src}/d" -e "/${marker}.*$/q" ${f} | \
		$sed -n -e \
		"s/^\($x\{8\}\) .*$s$x\{8\} \(\(0xf0 \)\?${key}\)$/\1 \2/p"`
		if [ $? != 0 ] ; then
			echo "${0}:\`${f}':\`${key}':search failed" 1>&2
			exit 1
		fi
		if [ -n "${line}" ] ; then
			break
		fi
	done &&
	$rm -f ${sym}.${src}[0-9]*
fi
if [ $? != 0 ] || [ -z "${line}" ] ; then
	echo "${0}:\`${sym}':\`${src}::${key}':sym failed to find" 1>&2
	exit 1
fi

# "${line}" is either
# "<hexaddr> <key>", or
# "<hexaddr> 0xf0 <key>" for mips16 text symbols
addr=${line%% *}
remain=${line#${addr}}
remain=${remain## }
addr="0x${addr}"
if [ $((addr)) = 0 ] ; then
	echo "${0}:${sym}:\`${line}':<addr> invalid" 1>&2
	exit 1
fi
if [[ "x${remain}" = x0xf0* ]] ; then
	# mips16 text addr + 1
	((++addr)) &&
	$rm -f ${sym}.${src}.${key}.bak &&
	echo "${addr}" | $t2b -b 4 - ${sym}.${src}.${key}.bak &&
	addr=`$od -t x4 ${sym}.${src}.${key}.bak | \
			$head -1 | $cut -d ' ' -f 2` &&
	$rm -f ${sym}.${src}.${key}.bak &&
	addr="0x${addr}"
	if [ $? != 0 ] || [ $((addr)) = 0 ] ; then
		echo "${0}:${sym}:\`${addr}':bad mips16 <addr>" 1>&2
		exit 1
	fi
fi

echo "${addr}"
