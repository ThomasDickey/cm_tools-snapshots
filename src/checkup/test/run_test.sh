#!/bin/sh
# $Id: run_test.sh,v 11.0 1991/10/22 10:29:17 ste_cm Rel $
date
#
# run from test-versions:
PATH=:`pwd`:`cd ../bin;pwd`:`cd ../../../bin;pwd`:/bin:/usr/bin:/usr/ucb
export PATH
#
TTY=/tmp/test$$
rm -rf junk
trap "rm -rf junk; rm -f $TTY" 0
#
mkdir junk
cd junk
rm -f junk.* RCS/junk.*
#
cp ../Makefile junk.txt
echo 'test file'>>junk.desc
#
cat <<eof/
**
**
**	Case 1.	Shows junk.desc (which is not checked-in).
eof/
checkin -q -u -tjunk.desc junk.txt
checkup junk.* >>$TTY
#
cat <<eof/
**
**
**	Case 2.	Shows junk.txt (which is assumed to have changes), and suppress
**		junk.desc using the -x option.
eof/
checkout -q -l junk.txt
touch junk.txt
checkup -x.desc junk.* >>$TTY
#
#
cat <<eof/
**
**
**	Case 3.	Traverses the local tree, suppressing ".out" and ".desc" files.
**		Again, junk.txt is reported.
eof/
checkup -x.out -x.desc .. >>$TTY
rm -f junk.* RCS/junk.*
cd ..
