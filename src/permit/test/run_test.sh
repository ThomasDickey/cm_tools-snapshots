#!/bin/sh
# $Id: run_test.sh,v 11.1 1992/10/30 08:20:04 dickey Exp $'
# run regression test for RCS permission utility
#
date
#
# run from test-versions:
rcstool=`which rcs`
for n in .. ../../.. ../../checkin
do	PATH=`cd $n/bin;pwd`:$PATH
done
PATH=:`pwd`:$PATH
export PATH
#
Q="-q"
RCS_DIR=FOO;export RCS_DIR
RCS_DEBUG=0;export RCS_DEBUG
USER=`whoami`;export USER

cat <<eof/
**
**	Creating a subdirectory, 'junk' which has one archived file, 'dummy'
**	The file-owner, "$USER" and "no_dummy" are the only ones on the
**	access list.  If this test is run with the RCS directories (or symbolic
**	links) intact for the current directory, those names will be shown in
**	the subsequent cases.
**
**	To make this test independent of the configuration, the RCS directories
**	are named "$RCS_DIR".
eof/
rm -rf junk
mkdir junk junk/RCS
cp Makefile junk/dummy
cd junk
touch null
checkin $Q -tnull dummy
$rcstool $Q -a$USER,no_dummy $RCS_DIR/dummy,v
cd ..
permit junk

cat <<eof/
**
**	Review the test-directory just created; there should be no actions
**	needed for it:
eof/
permit -n junk

cat <<eof/
**
**	Case 1:	Shows the current 'permit' state of the local directory.
**		If 'permit' has not been run (unlikely) then it would invoke
**		'rcs' to tidy up.
eof/
permit -n

cat <<eof/
**
**	Case 2:	Shows the current 'permit' state of the parent directory.
eof/
permit -n ..

cat <<eof/
**
**	Case 3:	Shows what would be done to purge permissions from the
**		current directory.
eof/
permit -np

cat <<eof/
**
**	Case 4:	Shows what would be done to purge permissions on the parent
**		directory (including this one).
eof/
permit -np ..

cat <<eof/
**
**	Case 5:	Shows what would be done to purge your permissions from
**		this directory.  Assumes that "$USER" is your userid.
eof/
permit -ne$USER

rm -rf junk
