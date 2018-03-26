#!/bin/sh
# configure project-specific settings of Sunplus SP5K platform
# It will
# 1. Call "make clean_resource" to clean old project resource
# 2. Call "sed" to change project-specific settings in
# makefile, solution_def.h, dev_init.h
#
# SOP for adding new model (add a new project: zz6336n)
# 1. change config.sh
#	find old project (ex vc6335n) with similar hw and duplicate and change to zz6335n (at least two places)
# 2. search whole project for HW_VC6335N/PRJ_VC6335N (depend on your new project)
#   check if you need to add defined(PRJ_ZZ6336N) code
# 3. copy project/vc6336n.def to project/zz6335n.def and
#	make necessary change (folder name, project files, PRJ_LIB (libhost3a.a...) ...)
# 4. copy resource/vc7335n to resource/zz6335n
#	modify resource/zz633n/frm_user.ini: FirmwareName = VC7335N.BIN 
#
# Adding illustration for updated new model SOP Usage
# 1.Project name ane hardware definition are fixed for one project. 
# 2.HW_VER definition is to be used.
# 3.HW_VER represents different stage of HW,ex:A1,A2,B1,B2...
# It can be used for different adc key/gpio/hw related issue handling.
# 4.The latest version of HW kit is with no HW_VER definition.
#	
# GQ add: Auto set flash align size,if you not input flash type,default to SPI.
#				 align size: SPI  --> 4KB
#										 NAND --> 128KB
# usage: config.sh prj 6330 spi
#    or: config.sh evb 6330 -->defult to SPI  
#    or: config.sh nand  -->prj project
#			

prefix=`dirname $0`

check_diff () {
	diff --version > /dev/null
	if [ $? != 0 ]; then
		echo ""
		echo "Cannot find diff!"
		echo "Please install diffutils at first!"
		echo ""
		exit 1
	fi
}

clean () {
	echo "Clean Host..."
	make clean > /dev/null
}

# $1:project
platform_config () {
	chip=$($prefix/../tool/makekeyget.sh project/$1.def CHIP)
	if [ -n "$cfgchip" ] ; then
		chip=$cfgchip
	fi
	oldchip=$($prefix/helper.sh chip)
	if [ "$chip" = "5210" ] ; then
		nandtime=0x01
		platform_path=5210
	else # 5310
		nandtime=0x13
		platform_path=spca$chip
	fi
	if [ ! -f ../$platform_path/tool_chain.def ] ; then
		echo $1 $chip environment not available!
		exit 1
	fi
	if [ "$chip" != "$oldchip" ] ; then
		platform_changed=YES
	fi
}

hardware_config () {
	dramsize=0x400000
	ddr2=YES
	case $hw_with_ver in
	CATHY)
		;;
	"720BF")		
		;;		
	HS)
		;;	
	EVB)
		;;	
	DVS5M2)
		;;
	
	*)
		echo "unknown hw:" $hw_with_ver
		exit 1
		;;
	esac
}

flash_config() {
	prjFile=./project/$(echo $1 | tr '[a-z]' '[A-Z]').def
	alignFile=../tool/FRM/ResImageMaker/config_template.xml
	
	if [ $# -ne 2 ] ; then
		echo error:invaild param!
	else	
		case $2 in
			spi)
			sed -i '/DHW_DISK_NAND/ s/^\s*CFLAGS/\t#CFLAGS/g' $prjFile
			sed -i '/DHW_DISK_SPI/ s/^\s*#CFLAGS/\tCFLAGS/g' $prjFile
			sed -i '/.*FirstDisk.*alignSize=\"[0-9]*\".*/ s/alignSize=\"[0-9]*\"/alignSize=\"4\"/g' $alignFile
			sed -i '/.*SecondDisk.*alignSize=\"[0-9]*\".*/ s/alignSize=\"[0-9]*\"/alignSize=\"128\"/g' $alignFile
			;;
			nand)
			sed -i '/DHW_DISK_NAND/ s/^\s*#CFLAGS/\tCFLAGS/g' $prjFile
			sed -i '/DHW_DISK_SPI/ s/^\s*CFLAGS/\t#CFLAGS/g' $prjFile
			sed -i '/.*alignSize=\"[0-9]*\".*/ s/alignSize=\"[0-9]*\"/alignSize=\"128\"/g' $alignFile
			;;
			*)
			echo "flash select err!!"
			;;
		esac
	fi	
}

platform_info(){
	prjFile=./project/$(echo $1 | tr '[a-z]' '[A-Z]').def
	
	sensor=$(cat $prjFile | grep '^\s*IQ_NAME\s*')
	sensor=${sensor##* }
	echo sensor:$sensor
	
	panel=$(cat $prjFile | grep '^\s*CFLAGS\s*+=\s*-DSP5K_LC.*' $prjFile)
	panel=${panel##*-D}
	panel=${panel%=*}
	echo panel :${panel:9}
	
	flash=$(cat $prjFile | grep '^\s*CFLAGS\s*+=\s*-DHW_DISK.*')
	flash=${flash##*_}
	flash=${flash%=*}
	echo flash :$flash
}

# only use for 6330M,$1:project name $2: 6330M flag,1(open),0(close)
chip_config(){
	prjFile=./project/$(echo $1 | tr '[a-z]' '[A-Z]').def
	
	if [ $2 == 1 ];then
		sed -i '/HW_CHIP/ s/^\s*#HW_CHIP/\tHW_CHIP/g' $prjFile
	else
		sed -i '/HW_CHIP/ s/^\s*HW_CHIP/\t#HW_CHIP/g' $prjFile
	fi	
}

software_config() {
# hw_facetrack
	hw_facetrack=YES

# hw_dist
#	hw_dist=NO
#	case $2 in # hw
#	VCA340T)
#		hw_dist=YES
#		;;
#	esac
}

configure () {
	prj=`echo $1 | tr '[a-z]' '[A-Z]'` # ucase
	hw=`echo $2 | tr '[a-z]' '[A-Z]'` # ucas
	hw_ver=`echo $3 | tr '[a-z]' '[A-Z]'` # ucase
	hw_with_ver=$hw
	if [ -n "$hw_ver" ] ; then
		hw_with_ver=${hw}_$hw_ver
	fi

	#echo prj=$prj
	#echo hw=$hw
	#echo hw_ver=$hw_ver
	#echo hw_with_ver=$hw_with_ver

	platform_config $prj
	hardware_config $hw $prj
	software_config $hw $prj

#
	target=include/customization/solution_def.h
	echo Update $target
	$prefix/../tool/ckeyset.sh $target "_\(FPGA\|EVB_SPE7001\)_HW_"
	$prefix/../tool/ckeyset.sh $target _CUSTOM_HW_ 1

#
	target=host.def
	echo Update $target
	$prefix/../tool/makekeyset.sh $target HW_DIST $hw_dist &&
	$prefix/../tool/makekeyset.sh $target HW_FACETRACK $hw_facetrack &&

#
	target=project/$prj.def
	if [ -n "$cfgchip" ] ; then
		echo Update $target
		$prefix/../tool/makekeyset.sh $target CHIP $chip
	fi
	$prefix/ubgen.sh $prj
#
	echo "PRJ := $prj" > lastprj.def
	hw_ver_old=$($prefix/helper.sh hardware_version)
	if [ -n "$hw_ver" ] ; then
		echo "HW_VER := $hw_ver" > last_hwver_$prj.def
	else
		rm -f last_hwver_$prj.def
	fi
	hw_ver=$($prefix/helper.sh hardware_version)

	if [ -n "$cfgchip" ] || [ "$hw_ver_old" != "$hw_ver" ] ; then
		clean $clean_host
	fi
	
}

# after branch, remove all extended project like PL/RO/MU...

main () {
	cfgname=$1
	
#	cfgver=${cfgname##*_}
#	if [ -z "$cfgver" ] ; then
#		cfgname=${cfgname%_*}
#	elif [ "$cfgver" = "$cfgname" ] ; then
#		cfgver=
#	else
#		cfgname=${cfgname%_*}
#	fi

	case $cfgname in
		prj)
		cfgname=prj_cvr
		;;
		evb)
		cfgname=evb_cvr
		;;
		*)
		;;
	esac

	cfgver=
	
	if [ $2 == "6330m" -o $2 == "6330M" ];then
		cfgchip=6330
		chip_config $cfgname 1
	else
		cfgchip=$2
		chip_config $cfgname 0
	fi
	
	flashtype=$(echo $3 | tr '[A-Z]' '[a-z]')
	
	flash_config $cfgname $flashtype
	#platform_info $cfgname

	case $cfgname in
		cathy)
		configure $cfgname cathy $cfgver
		;;
		720bf)
		configure $cfgname 720bf $cfgver
		;;
		evb_dsc)
		configure $cfgname evb $cfgver
		;;
		evb_cvr)
		configure $cfgname evb $cfgver
		;;
		rdk)
		configure $cfgname evb $cfgver
		;;
		prj_dsc)
		configure $cfgname evb $cfgver
		;;
		prj_cvr)
		configure $cfgname evb $cfgver
		;;
		hs)
		configure $cfgname hs $cfgver
		;;
		dvs5m2)
		configure $cfgname dvs5m2 $cfgver
		;;		
	.|?)
		;;
	*)
		echo "Usage: $0 [project [chip]"
		echo "  Available projects:"
		echo "  Cathy : 1M OV9712 (TBD) "
		echo "  720bf : 5M OV5633 (TBD) "
		echo "  Holystone : MT9V136 (TBD) "
		echo "  Available options for EVB:"
		echo "  evb : EVB HW+OV5610(5M) and previous project"		
		echo "  rdk : EVB HW+MT9J003(10M) and previous project"
		echo "	prj : Default project(If you not input any parameters,use it)!"	
		echo "	dvs5m2 : dvs5m2 (6330M + OV5653 + WIFI)"		
		echo
		;;

	esac
	echo "Current project: $($prefix/helper.sh project) [ $($prefix/helper.sh hardware) $($prefix/helper.sh hardware_version) $($prefix/helper.sh chip) ]"
}

check_diff

if [ $# -eq 1 ];then
	main prj 6330 $1
elif [ $# -eq 2 ];then
	main $1 $2 spi	
elif [ $# -eq 3 ];then
	main $1 $2 $3	
else
	main prj 6330 spi
fi

#Auto run iconPos.bat
cd ./script >/dev/null
./iconPos.bat > /dev/null
cd - >/dev/null
echo Generator icon position info done
exit
