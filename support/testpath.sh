#!/bin/sh
# $Id: testpath.sh,v 11.3 1992/11/24 08:49:45 dickey Exp $
#
#	"Pipe" this to setup test-related path
#
TOP=../../..
PATH=`cd $TOP/support;pwd`:`cd ../bin;pwd`:`cd $TOP/bin;pwd`:$PATH
for n in $*
do	d=../../$n/bin
	if test -d $d
	then	PATH=`cd $d;pwd`:$PATH
	fi
done
PATH=`pwd`:$PATH
echo $PATH
