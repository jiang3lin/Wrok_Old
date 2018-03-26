#!/bin/bash
if [ $# -gt 0 ] ; then
	prj=$(echo $1 | tr '[a-z]' '[A-Z]')
else
	prj=$(./helper.sh project)
fi

flash=$(cat ./project/$prj.def | grep "^\s*CFLAGS.*-DHW_DISK_.*")
flash=${flash##*_}
flash=${flash%=*}

case $flash in
	SPI)
	echo SPI
	_$prj/download/frm.exe &
	;;
	NAND)
	echo NAND
	_$prj/download/autofrm.exe &
	;;
	*)
	;;
esac
