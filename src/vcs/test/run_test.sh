#!/bin/sh
# $Id: run_test.sh,v 11.0 1991/10/22 12:28:41 ste_cm Rel $
# test-script for RCS baseline utility
#
date
#
# run from test-versions:
PATH=:`pwd`:`cd ../bin;pwd`:`cd ../../../bin;pwd`:/bin:/usr/bin:/usr/ucb
export PATH
#
JUNK='foo* FOO *.tst'
TTY=/tmp/test$$
CLEANUP="rm -rf $JUNK; rm -f $TTY"
RCS_DIR=FOO;export RCS_DIR
RCS_DEBUG=0;export RCS_DEBUG
#
echo '** initializing tests'
trap "$CLEANUP" 0
$CLEANUP
mkdir FOO
permit -q -b2 .
#
echo '** Testing INSERT'
#rm -f insert.tst
vcs -i foo/src foo/test foo2 >insert.tst 2>&1
echo '** resulting tree:' >>insert.tst
find $JUNK -print         >>insert.tst
#
compare.sh insert
#
echo '** Testing UNLOCK'
cp compare.sh foobar
checkin -q -l -tMakefile foobar 2>$TTY
vcs -u foobar             >unlock.tst    2>&1
#
compare.sh unlock
#
echo '** Testing DELETE'
vcs -d foo/src foo2 foo >delete.tst 2>&1
echo '** resulting tree:' >>delete.tst
find $JUNK -print         >>delete.tst
#
compare.sh delete
