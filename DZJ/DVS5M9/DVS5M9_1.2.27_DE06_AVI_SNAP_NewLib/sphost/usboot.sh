#!/bin/bash

echo USB boot..

#Utility path definition
usbboot=C:/Progra~1/Sunplus/UsbBoot/usbboot.exe
prefix=`dirname $0`

pushd $prefix/_DVS5M2/download/
#Dram parameter download
./SetPara.exe -I ./frm_user.ini
#Customized register set
./SetReg.exe -I ./SetReg.ini
#F/W bin file download
$usbboot 0xa0000000 ./DVS5M2.BIN
fmtime=`stat -c %y ./DVS5M2.BIN`
popd
echo
echo "F/W compiled at $fmtime"
cutime=`date "+%F %T.%N %z"`
echo "Current time is $cutime"
