#!/bin/sh
# $Id: run_test.sh,v 10.0 1991/10/18 07:48:37 ste_cm Rel $
# test-script for RCS checkin-package.  Cannot really test setuid mode from
# a dumb script, but this does at least verify that the code is "sane".
#
date
rm -rf junk
mkdir junk
cp Makefile junk/dummy
cd junk
PROG=../../bin/checkin
#
touch null_description

cat <<eof/
**
**	Show the original date for the new file:
eof/
ls -l dummy

cat <<eof/
**
**	Archive the file:
eof/
$PROG -u -tnull_description dummy

cat <<eof/
**
**	Show the resulting date for the new file:
eof/
ls -l dummy

cat <<eof/
**
**	Show the RCS log for the new file 'dummy':
eof/
rlog dummy
#
cd ..
rm -rf junk
