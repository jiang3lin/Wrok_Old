#!/bin/bash
# release cathyware lib
# GQ 2013 08.02

#***********************************************************************
# 											Env config
#***********************************************************************
ROOT=$(cd ../;pwd)
HOST=$ROOT/sphost

MAKE="make -C"
DEFFILE=./host.def

# defult project is PRJ
if [ -s $HOST/lastprj.def ];then
	PRJ=$(awk '{print $3}' $HOST/lastprj.def)
else
	PRJ=PRJ
fi

PRJDEF=$HOST/project/$PRJ.def

# record IQ name
IQ_NAME=$(awk '{if($1=="IQ_NAME") print $3}' $PRJDEF)

# record AWB choice
AWB=$(awk '{if($1~/^\s*ALGORITHM.*_AWB/) print $1}' $PRJDEF)
AWB=${AWB%_*}
AWB=${AWB#*_}

# path
ICATWARE_ROOT_PATH=$HOST/customization/hostfw/cathyware/src
CATHY_LIB_DOWNLOAD=$HOST/_$PRJ/_spca6330/customization/hostfw/cathyware/src
CATHY_LIB_PATH=$HOST/customization/hostfw/cathyware/lib

#***********************************************************************
# 											control the -Werror
#***********************************************************************

closeWerror(){
	sed -i '/^\s*CFLAGS\s*+=\s*-Werror\s*/ s/CFLAGS/#CFLAGS/g' $DEFFILE
}

openWerror(){
	sed -i '/^\s*#CFLAGS\s*+=\s*-Werror\s*/ s/#CFLAGS/CFLAGS/g' $DEFFILE
}

#***********************************************************************
# 											parameter check
#***********************************************************************
paraChk(){
	while [ -n "$1" ]
	do
		case $1 in
			ae | awb | md | iq | diq | com | all | --help)
			;;
			*)
			echo unkonw option: \"$1\"
			echo try \"$0 --help\" for more information.
			exit 1
			;;
		esac
		shift	
	done
}

#***********************************************************************
# 											parse parameters
#***********************************************************************
paraPrs(){
		case $1 in
			ae)
			BLDLIB=app_aealg.a
			;;
			awb)
			BLDLIB=app_awbalg.a
			;;
			md)
			BLDLIB=app_md.a
			;;
			iq)
			BLDLIB=lib_app_iq.a
			;;
			diq)
			BLDLIB=app_diq.a
			;;
			com)
			BLDLIB=app_icatware_common.a
			;;
			all)
			cathylib ae awb md iq diq com
			;;
			--help)
			echo usage: $0 [option]...
			echo option: ae,awb,md,iq,diq,com,all.
			echo
			echo "	ae,	app_aealg.a" 
			echo "	awb,	app_awbalg.a"
			echo "	md,	app_md.a"	
			echo "	iq,	lib_app_iq.a"			
			echo "	diq,	app_diq.a"	
			echo "	com,	app_icatware_common.a"
			echo "	all,	build all libs"
			openWerror 
			exit 0  
			;;
			*)
			;;
		esac	
}

#***********************************************************************
# 											build cathyware lib
#***********************************************************************
cathylib(){	
# build and release cathyware lib
  BLDLIB=
	while [ -n "$1" ]
	do
		paraPrs $1
		
		case $BLDLIB in
			app_aealg.a)
			LIBPATH=$CATHY_LIB_DOWNLOAD/aealg
			$MAKE $ICATWARE_ROOT_PATH/aealg -j > /dev/null
			;;
			app_awbalg.a)
			if [ "$AWB" == "ALGORITHM" ];then
				LIBPATH=$CATHY_LIB_DOWNLOAD/awbalg/awbalg
				$MAKE $ICATWARE_ROOT_PATH/awbalg/awbalg -j > /dev/null
			else
				LIBPATH=$CATHY_LIB_DOWNLOAD/awbalg/awb$(echo $AWB | tr '[A-Z]' '[a-z]')alg
				$MAKE $ICATWARE_ROOT_PATH/awbalg/awb$(echo $AWB | tr '[A-Z]' '[a-z]')alg -j > /dev/null
			fi
			;;
			app_diq.a)
			LIBPATH=$CATHY_LIB_DOWNLOAD/diq
			$MAKE $ICATWARE_ROOT_PATH/diq -j > /dev/null
			;;
			app_icatware_common.a)
			LIBPATH=$CATHY_LIB_DOWNLOAD
			$MAKE $ICATWARE_ROOT_PATH -j > /dev/null
			;;
			app_md.a)
			LIBPATH=$CATHY_LIB_DOWNLOAD/md
			$MAKE $ICATWARE_ROOT_PATH/md -j > /dev/null
			;;
			lib_app_iq.a)
			echo sensor:$IQ_NAME
			LIBPATH=$CATHY_LIB_DOWNLOAD/iq
			$MAKE $ICATWARE_ROOT_PATH/iq -j > /dev/null
			;;
			*)
			;;
		esac
		
		if [ $? != 0 ];then
			echo "'$BLDLIB' build error!!! "
			openWerror
			exit 1
		else
			if [ "$BLDLIB" == "lib_app_iq.a" ];then
				cp -f $LIBPATH/$BLDLIB $CATHY_LIB_PATH/$IQ_NAME/
			else
				cp -f $LIBPATH/$BLDLIB $CATHY_LIB_PATH/
			fi
			echo $BLDLIB build success!!
			echo		 
		fi
	shift	
	done		
}

main(){
	paraChk $*
	closeWerror
	cathylib $*
	openWerror
}

main $*