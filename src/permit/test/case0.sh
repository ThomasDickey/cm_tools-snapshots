#!/bin/sh
# $Id: case0.sh,v 11.2 1997/09/14 21:51:07 tom Exp $'
#
#	Initialize tests for 'permit'
#
#	Creating a subdirectory, 'junk' which has one archived file, 'dummy'
#	The file-owner, "$USER" and "no_dummy" are the only ones on the
#	access list.  If this test is run with the RCS directories (or symbolic
#	links) intact for the current directory, those names will be shown in
#	the subsequent cases.
#
#	To make this test independent of the configuration, the RCS directories
#	are named "$RCS_DIR".
#
rm -rf junk
mkdir junk junk/RCS
cp makefile.in junk/dummy
cd junk
touch null
checkin $Q -tnull dummy
run_tool rcs $Q -a$USER,no_dummy $RCS_DIR/dummy,v
cd ..
permit junk
#
#
#	Review the test-directory just created; there should be no actions
#	needed for it:
#
permit -n junk
