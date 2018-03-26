#!/bin/bash


#
# Change the following by you own
#
PLATFORM=spca6330
SRC_FLAG="install V33=YES -j"
HOST_FLAG="RES=NO V33=YES -j"
#PRE_CMD="touch customization/sp5k_job.c"
#POST_CMD="./down.exe _RDK/download/RDK.bin"

#
# You don't have to change the following
#
SRC_PATH=../${PLATFORM}/src
SEARCH_PATH="middleware middleware/drivers api"



if [[ -z $1 ]]; then
exit 1
fi

sp="[[:blank:]]"
w="[[:alnum:]]"

while [[ ! -z $1 ]]
do
	#clear the target at first
	target=""
	case $1 in
		host)
			break	
			;;
		src | all)
			target=${SRC_PATH}
			;;
		os)
			target=${SRC_PATH}/os
		;;
		hal)
			target=${SRC_PATH}/hal
		;;
		modesw | gfx | pip | fs | slide)
			target=${SRC_PATH}/middleware/$1
		;;
		5kmodesw | 5kgfx | 5kglobal | 5kpip | 5kfs | 5kslide)
			suffix=`echo $1 | sed -e "s/5k//"`
			target=${SRC_PATH}/api/$suffix
		;;
		-c)
			SRC_CLEAN="clean V33=YES -j"
			shift
			continue
			;;
		-C)
			HOST_CLEAN="clean V33=YES -j"
			shift
			continue
			;;
		-r)
			HOST_FLAG=${HOST_FLAG/RES=NO}
			shift
			continue
			;;
		-u)
			HOST_FLAG="${HOST_FLAG/-j}usboot=1 -j"
			POST_CMD="./usboot.sh"
			shift
			continue
			;;
		-f)
			POST_CMD="./frm.sh"
			shift
			continue
			;;
		*)
		for search in ${SEARCH_PATH}
		do
			ITEMS=`sed -ne "/^SUBDIRS/s/^SUBDIRS${sp}*+=${sp}*\(${w}\)/\1/p" ${SRC_PATH}/${search}/Makefile`
			for item in ${ITEMS}
			do
				if [[ "$1" == ${item} ]]; then
					target=${SRC_PATH}/${search}/${item}
					break;
				fi
			done
		done
		;;
	esac

	if [[ ! -z ${target} ]]; then	
		cd ${target}
		echo ""
		echo "[Compile] Enter ${target} "
		if [[ ! -z ${SRC_CLEAN} ]]; then
			echo "make ${SRC_CLEAN}"
			make ${SRC_CLEAN}
			if [[ $? != 0 ]]; then
			exit 1
			fi
		fi
		echo "make ${SRC_FLAG}"
		make ${SRC_FLAG}
		if [[ $? != 0 ]]; then
			exit 1
		fi
		cd -
	else
		echo "[Compile] Cannot found - $1 -"
		exit 1
	fi
	
	shift

done

if [[ ! -z ${PRE_CMD} ]]; then
	echo ${PRE_CMD}
	${PRE_CMD}
	if [[ $? != 0 ]]; then
		exit 1
	fi
fi

if [[ ! -z ${HOST_CLEAN} ]]; then
	echo "make ${HOST_CLEAN}"
	make ${HOST_CLEAN}
	if [[ $? != 0 ]]; then
	exit 1
	fi
fi

echo "make ${HOST_FLAG}"
make ${HOST_FLAG}
if [[ $? != 0 ]]; then
	exit 1
fi

if [[ ! -z ${POST_CMD} ]]; then
	echo ${POST_CMD}
	${POST_CMD}
fi
