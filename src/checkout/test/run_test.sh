#!/bin/sh
# $Id: run_test.sh,v 11.1 1992/10/28 12:08:18 dickey Exp $
# test-script for RCS checkout utility
#
# run from test-versions:
for n in .. ../../.. ../../checkin
do	PATH=`cd $n/bin;pwd`:$PATH
done
PATH=`pwd`:$PATH
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
run_tool rlog dummy

cat <<eof/
**
**	The checked-out file:
eof/
ls -l dummy
#
cd ..
rm -rf junk
