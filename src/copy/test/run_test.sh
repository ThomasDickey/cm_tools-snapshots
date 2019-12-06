#!/bin/sh
# $Id: run_test.sh,v 11.2 2019/12/02 21:58:04 tom Exp $
# test unix copy utility be copying some files and directories; should have
# no differences.
#
date
#
# run from test-versions:
for p in . ../bin ../../bin ../../../bin
do
	[ -d "$p" ] && PATH=`unset CDPATH;cd $p && pwd`:$PATH
done
export PATH
#
rm -rf junk pile
mkdir junk junk/subdir1 junk/subdir2 junk/subdir1/nuisance
cp makefile.in junk/first
copy makefile.in junk/second
copy junk pile
ls -lR junk | sed -e s/junk/NAME/ >junk.out
ls -lR pile | sed -e s/pile/NAME/ >pile.out
if ( cmp -s junk.out pile.out )
then
	echo '** normal test'
else
	echo '?? difference found'
	diff junk.out pile.out
fi
#
rm -rf junk pile junk.out pile.out
