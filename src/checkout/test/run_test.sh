#!/bin/sh
# $Id: run_test.sh,v 11.0 1991/10/18 08:01:44 ste_cm Rel $
# test-script for RCS checkout utility
#
# run from test-versions:
logtool=`which rlog`
PATH=:`pwd`:`cd ../bin;pwd`:`cd ../../../bin;pwd`:/bin:/usr/bin:/usr/ucb
export PATH
#
date
rm -rf junk
mkdir  junk
cp Makefile junk/dummy
cd junk
#
cat <<eof/
**
**	Creating a copy of 'Makefile', checked-in via -k (keys) option.
**	This will preserve the original checkin-date.
eof/
touch null_description
checkin -k -q -tnull_description dummy

cat <<eof/
**
**	Checking-out a copy of the resulting file 'dummy'
eof/
rm -f dummy

cat <<eof/
**
**	The resulting RCS log:
eof/
checkout dummy
$logtool dummy

cat <<eof/
**
**	The checked-out file:
eof/
ls -l dummy
#
cd ..
rm -rf junk
