#!/bin/sh
# $Id: run_test.sh,v 11.1 1997/09/14 21:34:46 tom Exp $
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
