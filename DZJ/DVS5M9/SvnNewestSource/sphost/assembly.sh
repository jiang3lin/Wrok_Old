#!/bin/bash

if [[ -z $1 ]]; then
	echo "Usage: assembly.sh address [offset]"
	exit 1
fi
if [[ -z $2 ]]; then
	offset=0x80
else
	offset=$2
fi

prj=$(./helper.sh project)
chip=$(./helper.sh chip)

../tool/objdump.exe -d -S --start-address=$(($1-$offset)) --stop-address=$(($1+$offset)) _$prj/spca$chip.elf
