#!/bin/sh
# $Id: run_test.sh,v 11.3 1993/04/27 11:28:15 dickey Exp $
# test-script for RCS checkout utility
#
# run from test-versions:
PATH=`../../../support/testpath.sh checkin`; export PATH
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
