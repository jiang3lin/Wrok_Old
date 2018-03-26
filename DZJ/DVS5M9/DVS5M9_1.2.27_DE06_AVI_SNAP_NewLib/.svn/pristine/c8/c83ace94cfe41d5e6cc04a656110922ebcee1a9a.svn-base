#!/bin/sh
prj=$(./helper.sh project)
chip=$(./helper.sh chip)

elf=_$prj/spca$chip.elf

if [ $# = 0 ] ; then
	../tool/objdump -d $elf | less
	exit
fi

until [ -z "$1" ]  # Until all parameters used up . . .
do
	../tool/objdump -t $elf | grep -w "\.\(text\|rom\)\(\.${w}\+\)\?" | \
	cut -c 1-9,32- | sort | \
	../tool/addr2func.exe $1  $2  $3  $4  $5  $6  $7  $8  $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19} | sort
	if [ $# -gt 19 ] ; then
		shift 19
	else
		shift $#
	fi
done
