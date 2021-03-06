#!/bin/sh
# $Id: run_test.sh,v 11.4 1997/09/14 21:33:32 tom Exp $
# test-script for RCS checkout utility
#
# run from test-versions:
PATH=`../../../support/testpath.sh checkin`; export PATH
#
date
rm -rf junk
mkdir  junk
cp makefile.in junk/dummy
cd junk
#
cat <<eof/
**
**	Creating a copy of 'makefile.in', checked-in via -k (keys) option.
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
