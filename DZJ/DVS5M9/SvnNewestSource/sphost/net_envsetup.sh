#!/bin/bash
#Usage . ./envsetup.sh

export HOST_TOPDIR=`cd ..;pwd`
export HOST_DIR=`pwd`
export BASEFW_SRC_DIR=${HOST_TOPDIR}/spca6330/src
export TORTOISE_SVN="/cygdrive/c/Program Files/TortoiseSVN/bin/svn"

alias cdfw="cd ${HOST_TOPDIR}/spca6330/src"
alias cdmw="cd ${HOST_TOPDIR}/spca6330/src/middleware"

alias cdhost="cd ${HOST_DIR}"
alias cdnet="cd ${HOST_DIR}/customization/thirdparty/net"

function __make {
	CUR_DIR=`pwd`
	cd $1
	echo Enter: `pwd`
	echo make $2 $3 $4 $5 $6 $7 $8 $9
	#echo "Press any key to continue building"
	#read
	sleep 1
	echo "Start Making ..."
	make $2 $3 $4 $5 $6 $7 $8 $9
	cd ${CUR_DIR}
}

function mkhost {
	case $1 in
	net)
		__make ${HOST_DIR}/customization/thirdparty/net net $2 $3 $4 $5 $6 $7 $8 $9
		;;
	st)
		__make ${HOST_DIR} V33=YES WIFI=2 LIVEST=1 $2 $3 $4 $5 $6 $7 $8 $9
		;;
	*)
		__make ${HOST_DIR} V33=YES $1 $2 $3 $4 $5 $6 $7 $8 $9
	esac
}

function mkfw {
	case $1 in
	libav)
		__make ${BASEFW_SRC_DIR}/middleware/libav install V33=YES $2 $3 $4 $5 $6 $7 $8 $9
		;;
	*)
		__make ${BASEFW_SRC_DIR} install V33=YES $1 $2 $3 $4 $5 $6 $7 $8 $9
	esac
}

function frm {
	echo run "${HOST_DIR}/_EVB/download/frm.exe"
	"${HOST_DIR}/_EVB/download/frm.exe" &
}

function svn_update {
	cd ${HOST_TOPDIR}/spca6330
	"${TORTOISE_SVN}" update
	cd ${HOST_DIR}
	"${TORTOISE_SVN}" update
}

