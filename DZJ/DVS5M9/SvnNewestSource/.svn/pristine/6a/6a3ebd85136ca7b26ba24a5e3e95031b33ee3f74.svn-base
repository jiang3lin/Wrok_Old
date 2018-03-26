#!/bin/sh

prjdef=host.def
if [ -f lastprj.def ] ; then
	prjdef=lastprj.def
fi

prefix=`dirname $0`
prj=$($prefix/../tool/makekeyget.sh $prjdef PRJ)
chip=$($prefix/../tool/makekeyget.sh project/$prj.def CHIP)

if [ "$chip" = "5210" ] ; then
 platform_path=5210
else
 platform_path=spca$chip
fi

case $1 in
project)
	echo $prj
    ;;
host_version)
	host_ver=$(grep -F '/*VQ*/' customization/HostFw/APP/app_main.c | \
		$prefix/../tool/ckeyget.sh - APP_VERSION)
	if [ -z "$host_ver" ] ; then
		host_ver=0
	fi
	echo $host_ver
	;;
host_subversion)
	host_subver=$(grep -F '/*VQ*/' customization/HostFw/APP/app_main.c | \
		$prefix/../tool/ckeyget.sh - APP_SUB_VERSION)
	host_subver=$(printf "%02d" $host_subver)
	echo $host_subver
	;;
host_revision)
	host_rev=$(grep -F '/*VQ*/' customization/HostFw/APP/app_main.c | \
		$prefix/../tool/ckeyget.sh - APP_REVISION)
	if [ -z "$host_rev" ] ; then
		host_rev=0
	fi
	echo $host_rev
	;;

project_chip) # chip of specified project
	prj=${2%_*}
	$prefix/../tool/makekeyget.sh project/$prj.def CHIP
	;;

chip) # chip defined in project.def
	echo $chip
	;;
platform_path)
	echo $platform_path
	;;
base_version)
	base_ver_h=../$platform_path/inc/version.h
	if [ -f $base_ver_h ] ; then
		base_ver=$(sed -n -e '/#define BASE/s/#.*N//p' $base_ver_h | tr -d ' \t"')
	fi
	if [ -z "$base_ver" ] ; then
		base_ver=x.xx.xx
	fi
	echo $base_ver
	;;
hardware)
	$prefix/../tool/makekeyget.sh project/$prj.def HW_NAME
	;;
hardware_version)
	if [ -f "last_hwver_$prj.def" ] && [ -r "last_hwver_$prj.def" ] ; then
		hwver_def=last_hwver_$prj.def
	else
		hwver_def=project/$prj.def
	fi
	$prefix/../tool/makekeyget.sh $hwver_def HW_VER
	;;
sensor_vision)
	# sensor_vision = $1 
	# ResFiles		= $2~

	sed=/bin/sed
	sort=/usr/bin/sort

	SENSOR_NAME=$($prefix/../tool/makekeyget.sh project/$prj.def SENSOR_NAME)
	if [ -z "$SENSOR_NAME" ] ; then
		SENSOR_NAME=0
	else	
		AFE_NAME=$($prefix/../tool/makekeyget.sh project/$prj.def AFE_NAME)
		
		sensorname=`echo $SENSOR_NAME | tr '[A-Z]' '[a-z]'` # ucase
		afename=`echo $AFE_NAME | tr '[A-Z]' '[a-z]'` # ucase
	
	
		if [ -z "$afename" ] ; then
			afename=$sensorname
		fi
		
	
		sensor_Tg_drv=customization/$platform_path/drivers/sensor/model/sensor_tg_$sensorname.c
		sensor_Afe_drv=customization/$platform_path/drivers/sensor/model/sensor_afe_$afename.c
		
		MODEL_BUILD_DAY=20`date +%y%m%d`
		
	
		# 1. get model_res_version from sensor_TG_drv
		MODEL_RES_VERSION=$(grep -F '#define MODEL_VERSION "' $sensor_Tg_drv | cut -d '-' -f 2 | cut -d '.' -f 1)
		#echo MODEL_RES_VERSION : $MODEL_RES_VERSION
		
		# 2. get MODEL_DRV_VERSION from sensor_TG_drv
		MODEL_DRV_VERSION=$(grep -F '#define MODEL_VERSION "' $sensor_Tg_drv | cut -d '-' -f 2 | cut -d '.' -f 2)
		#echo MODEL_DRV_VERSION : $MODEL_DRV_VERSION
		
		# 3. get MODEL_SUB_VERSION from sensor_TG_drv
		MODEL_SUB_VERSION=$(grep -F '#define MODEL_VERSION "' $sensor_Tg_drv | cut -d '-' -f 2 | cut -d '.' -f 3 | cut -d '"' -f 1)
		#echo MODEL_SUB_VERSION : $MODEL_SUB_VERSION
	
	
		# 4. get AFE_RES_VERSION from sensor_Afe_drv
		AFE_RES_VERSION=$(grep -F '#define MODEL_VERSION "' $sensor_Afe_drv | cut -d '-' -f 2 | cut -d '.' -f 1)
		#echo AFE_RES_VERSION : $AFE_RES_VERSION
		
		# 5. get AFE_DRV_VERSION from sensor_Afe_drv
		AFE_DRV_VERSION=$(grep -F '#define MODEL_VERSION "' $sensor_Afe_drv | cut -d '-' -f 2 | cut -d '.' -f 2)
		#echo AFE_DRV_VERSION : $AFE_DRV_VERSION 
	
		if [ -z "$MODEL_RES_VERSION" ] ; then
			MODEL_RES_VERSION=00
		fi
		if [ -z "$MODEL_DRV_VERSION" ] ; then
			MODEL_DRV_VERSION=00
		fi
		if [ -z "$MODEL_SUB_VERSION" ] ; then
			MODEL_SUB_VERSION=00
		fi
		if [ -z "$AFE_RES_VERSION" ] ; then
			AFE_RES_VERSION=00
		fi
		if [ -z "$AFE_DRV_VERSION" ] ; then
			AFE_DRV_VERSION=00
		fi
		
		if [ "$MODEL_DRV_VERSION" != "$AFE_DRV_VERSION" -o "$MODEL_RES_VERSION" != "$AFE_RES_VERSION" ] ; then
			sed -i "s/\#define MODEL_VERSION \"\$unp_afe_.*$/\#define MODEL_VERSION \"\$unp_afe_$afename-$MODEL_RES_VERSION.$MODEL_DRV_VERSION.$MODEL_SUB_VERSION\"/g" $sensor_Afe_drv
			echo sync $sensor_Afe_drv from $res_file_version to $sensorname-$MODEL_RES_VERSION.$MODEL_DRV_VERSION.$MODEL_SUB_VERSION - $MODEL_BUILD_DAY
		fi
		
		#shift 1 to reduce $1 sensor_vision $2 $3
		shift 1
	
		#get files list from $2
		files=`echo $* | $sed -e "s/^$s\+//" -e "s/$s\+/\n/g" | $sort -u` &&
	
		# echo files $# $files
	    for res_file in ${files} ; do
			
	        if [ -z "$res_file" ] ; then
	          echo NO file !!!
	        else
	
			    res_file_version=$(grep -F -m 1 '/* MODEL_VERSION' $res_file | cut -d '-' -f 2 | cut -d '.' -f 1)
		
				if [ -z "$res_file_version" ] ; then
					res_file_version=00
				fi
				
				if [ "$MODEL_RES_VERSION" -gt "$res_file_version" ] ; then
					echo sync $res_file from $res_file_version to $sensorname-$MODEL_RES_VERSION.$MODEL_DRV_VERSION.$MODEL_SUB_VERSION - $MODEL_BUILD_DAY
					sed -i -e 1a"/* MODEL_VERSION $sensorname-$MODEL_RES_VERSION.$MODEL_DRV_VERSION.$MODEL_SUB_VERSION - $MODEL_BUILD_DAY */" $res_file
				fi	
	
			fi
		done
	fi
	;;		
hardware_version_list)
	hw=$2
	hwiopath=customization/HostFw/APP/Device
	hw_list=$(ls $hwiopath/HW_*.io | sed -ne "s|^$hwiopath/HW_$2_\(.*\)\.io$|\1|ip")
	echo $hw_list
	;;
*)
 	echo $prj $chip $platform_path
 	;;
esac
