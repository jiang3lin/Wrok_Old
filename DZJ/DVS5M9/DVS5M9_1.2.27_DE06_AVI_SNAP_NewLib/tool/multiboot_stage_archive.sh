#!/bin/sh
#/**************************************************************************
# *
# *        Copyright (c) 2010 by Sunplus mMedia Inc., Ltd.
# *
# *  This software is copyrighted by and is the property of Sunplus
# *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
# *  Inc., Ltd. This software may only be used in accordance with the
# *  corresponding license agreement. Any unauthorized use, duplication,
# *  distribution, or disclosure of this software is expressly forbidden.
# *
# *  This Copyright notice may not be removed or modified without prior
# *  written consent of Sunplus mMedia Inc., Ltd.
# *
# *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
# *  software without notice.
# *
# *  Sunplus mMedia Inc., Ltd.
# *  19-1, Innovation First Road, Science-Based Industrial Park,
# *  Hsin-Chu, Taiwan.
# *
# *  Author: Anmin Deng
# *
# **************************************************************************/

# to find the archive stage file used to build project multi boot ${1} stage
# ${1} is the multi boot stage value, eg, 5
# ${2} is the archive file name, eg, libmy.a
# result is the archive stage file name output to stdout, eg, libmy.stage5.a

# \example if we have libmy.stage1.a, libmy.stage5.a. libmy.stage.10.a,
# % ${0} 5 libmy.a  ---> will print "libmy.stage5.a" to stdout;
# % ${0} 3 libmy.a  ---> will print "libmy.stage1.a" to stdout;
# % ${0} 8 libmy.a  ---> will print "libmy.stage5.a" to stdout;

if [ "$#" != 2 ] ; then
	echo "${0}: usage: \`stage_val' \`archive'" 1>&2
	exit 1
fi

stage=${1}
lib=${2}

ls=/bin/ls
sed=/bin/sed
sort=/usr/bin/sort

set -o pipefail

# there are many '/'s in ${lib}, using a rare char in sed for delimiter
liblist=`$ls ${lib%.a}.stage*.a 2> /dev/null | \
	$sed -n -e "s|^${lib%.a}.stage\([0-9]\+\)\.a$|\1|p"`
# echo "liblist="${liblist} 1>&2

if [ -z "${liblist}" ] ; then
	echo "${lib}"
elif [ -n "`echo ${liblist} | grep -w ${stage}`" ] ; then
	echo "${lib%.a}.stage${stage}.a"
else
	inslist=`echo ${stage} ${liblist} | $sed -e 's/ /\n/g' | $sort -g -u`
	# echo "inslist="${inslist} 1>&2
	echo -n "${lib%.a}.stage"
	echo -n ${inslist} | $sed -n -e "s/.*\b\([0-9]\+\) ${stage}\b.*/\1/p"
	echo ".a"
fi
