#!/bin/sh
#/**************************************************************************
# *
# *      Copyright (c) 2010-2012 by Sunplus mMedia Inc., Ltd.
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

# to print multiboot loading info to stdout according to stage image sizes
# \param ${1} is the .elf file
# \param ${2} is the stagelist
# \param ${3}, the system building environment prefix path
# \param ${4}, optional, multiboot edges aligned to number of pages, default 1

if [ "$#" != 3 ] && [ "$#" != 4 ] ; then
	echo "${0}: usage: \`elf' \"\`stagelist'\" \`prefix' [\`npage']" 1>&2
	exit 1
fi

elf=${1}
stagelist=${2}
objdump=${3}objdump
if [ "$#" = 3 ] ; then
	npage=1
else
	npage=${4}
fi

if [ ! -f "${elf}" ] || [ ! -r "${elf}" ] ; then
	echo "${0}: \`${elf}': input can not open" 1>&2
	exit 1
fi

if [ ! -x "${objdump}" ] ; then
	echo "${0}: \`${objdump}': system path failed" 1>&2
	exit 1
fi

x=[[:xdigit:]]
s=[[:blank:]]
e=[[:space:]]

sed=/bin/sed
rm=/bin/rm

(( align = npage * 512 ))

# \param ${1} is the symbol string
# \param ${2} "up": to align up to page idx; "down": to align down to page idx;
# <pageidx>: to align up to page idx but not exceeding ${2} <pageidx>;
# N/A: no aligned address of symbol ${1}
find_symbol_addr()
{
	local sym=${1}
	local addr=`$sed -n -e "s/^8\($x\{7\}\)$s\+.*$s\+${sym}$e\?$/0x\1/p" \
			${elf}.sym.bak`
	if [ $? != 0 ] || [ -z "${addr}" ] || (( "${addr}" <= 0 )) ; then
		echo "${0}: \`${elf}': \`${sym}': symbol not found" 1>&2
		return 1
	fi

	if [ -n "${2}" ] ; then
		if [ "${2}" = "down" ] ; then
			(( addr = addr / align * npage ))
		elif [ "${2}" = "up" ] ; then
			(( addr = ( addr + align - 1 ) / align * npage ))
		else
			(( addr = ( addr + align - 1 ) / align * npage ))
			if (( "${addr}" > "${2}" )) ; then
				addr=${2}
			fi
		fi
	fi
	printf '%#x\n' ${addr}
}

# main

if [ "${elf}.sym.bak" -ot "${elf}" ] ; then
	$rm -f ${elf}.sym.bak
	${objdump} -t ${elf} > ${elf}.sym.bak
	if [ $? != 0 ] ; then
		echo "${0}: objdump: \`${elf}': failed" 1>&2
		exit 1
	fi
fi

# not supporting multiload, only to use FIC
if [ -z "${stagelist}" ] ; then
	echo "0" &&
	find_symbol_addr _fic_start &&
	echo -e "0\n0"
	if [ $? != 0 ] ; then
		echo "${0}: fic only failed" 1>&2
		exit 1
	fi
	exit
fi

stagearray=(${stagelist})
nstage=${#stagearray[*]}
stagemask=0

for stage in ${stagelist} ; do
	(( stagemask |= 1 << (stage - 1) ))
done

etext=`find_symbol_addr _etext down` &&
erodata=`find_symbol_addr _erodata down` &&
edata=`find_symbol_addr _edata down`
if [ $? != 0 ] || [ -z "${etext}" ] || (( "${etext}" <= 0 )) \
|| [ -z "${erodata}" ] || (( "${erodata}" <= 0 )) \
|| [ -z "${edata}" ] || (( "${edata}" <= 0 )) ; then
	echo "${0}:\`${elf}':_etext,_erodata,_edata:symbol invalid" 1>&2
	exit 1
fi

if (( "${etext}" > "${edata}" )) ; then
	etext=${edata}
fi
if (( "${erodata}" > "${edata}" )) ; then
	erodata=${edata}
fi

find_symbol_addr _multiboot_load_info &&
find_symbol_addr _fic_start &&
echo ${nstage} &&
printf '%#x\n' ${stagemask} &&
echo "${edata}" &&
find_symbol_addr _bssStart up &&
find_symbol_addr _multiboot_start down &&
echo "${etext}" &&
echo "${erodata}" &&
find_symbol_addr _bssStart
if [ $? != 0 ] ; then
	echo "${0}: beginning: invalid" 1>&2
	exit 1
fi

for stage in ${stagelist% [0-9]*} ; do
	find_symbol_addr _etext.stage${stage} ${etext} &&
	find_symbol_addr _erodata.stage${stage} ${erodata} &&
	find_symbol_addr _edata.stage${stage} ${edata} &&
	find_symbol_addr _ebss.stage${stage}
	if [ $? != 0 ] ; then
		echo "${0}: middle loop \`${stage}': invalid" 1>&2
		exit 1
	fi
done

echo "${etext}" &&
echo "${erodata}" &&
echo "${edata}" &&
find_symbol_addr _bssEnd
if [ $? != 0 ] ; then
	echo "${0}: tail: invalid" 1>&2
	exit 1
fi
