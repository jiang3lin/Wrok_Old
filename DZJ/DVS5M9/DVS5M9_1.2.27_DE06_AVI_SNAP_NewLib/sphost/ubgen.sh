#!/bin/bash

#Utility path definition
usb_boot=C:/Progra~1/Sunplus/UsbBoot/usbboot.exe
tar_script=usboot.sh

#Check parameter
if [ $# = 0 ] ; then 
	echo Usage: $0 \<ProjectName\> [ParameterFile]
	exit
fi

prj=$1
if [ -z "$2" ] ; then
	dpfile="-I ./frm_user.ini"
else
	dpfile=$2
fi

#Target script file generation
echo "#!/bin/bash"                                  >$tar_script
echo ""                                            >>$tar_script
echo "echo USB boot.."                             >>$tar_script
echo ""                                            >>$tar_script
echo "#Utility path definition"                    >>$tar_script
echo "usbboot=$usb_boot"                           >>$tar_script
echo "prefix=\`dirname \$0\`"                      >>$tar_script
echo ""                                            >>$tar_script
echo "pushd \$prefix/_$prj/download/"              >>$tar_script
echo "#Dram parameter download"                    >>$tar_script
echo "./SetPara.exe $dpfile"                       >>$tar_script
echo "#Customized register set"                    >>$tar_script
echo "./SetReg.exe -I ./SetReg.ini"                >>$tar_script
echo "#F/W bin file download"                      >>$tar_script
echo "\$usbboot 0xa0000000 ./$prj.BIN"             >>$tar_script
echo "fmtime=\`stat -c %y ./$prj.BIN\`"            >>$tar_script
echo "popd"                                        >>$tar_script
echo "echo"                                        >>$tar_script
echo "echo \"F/W compiled at \$fmtime\""           >>$tar_script
echo "cutime=\`date \"+%F %T.%N %z\"\`"            >>$tar_script
echo "echo \"Current time is \$cutime\""           >>$tar_script

#Set script file access mode
chmod 777 $tar_script