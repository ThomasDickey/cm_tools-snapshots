#!/bin/sh
# $Id: run_test.sh,v 11.0 1991/10/18 08:28:50 ste_cm Rel $
# test unix copy utility be copying some files and directories; should have
# no differences.
#
date
#
# run from test-versions:
PATH=:`pwd`:`cd ../bin;pwd`:`cd ../../../bin;pwd`:/bin:/usr/bin:/usr/ucb
export PATH
#
rm -rf junk pile
mkdir junk junk/subdir1 junk/subdir2 junk/subdir1/nuisance
cp Makefile junk/first
copy Makefile junk/second
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
