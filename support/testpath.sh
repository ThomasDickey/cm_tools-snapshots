#!/bin/sh
# $Id: testpath.sh,v 11.4 2019/12/02 22:03:48 tom Exp $
#
#	"Pipe" this to setup test-related path
#
for p in .. ../.. ../../..
do
	for q in bin support
	do
		[ -d $p/$q ] && PATH=`unset CDPATH; cd $p/$q && pwd`:$PATH
	done
done
for n in $*
do	d=../../$n/bin
	if test -d $d
	then	PATH=`cd $d;pwd`:$PATH
	fi
done
PATH=`pwd`:$PATH
echo $PATH
