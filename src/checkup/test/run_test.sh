#!/bin/sh
# $Id: run_test.sh,v 11.2 1992/10/30 07:52:41 dickey Exp $
date
#
# run from test-versions:
for n in .. ../../.. ../../checkin ../../checkout
do	PATH=`cd $n/bin;pwd`:$PATH
done
PATH=:`pwd`:$PATH
export PATH
#
TTY=/tmp/test$$
rm -rf junk
trap "rm -rf junk; rm -f $TTY" 0
#
if test -z "$RCS_DEBUG"
then
	RCS_DEBUG=0
	export RCS_DEBUG
fi
if test $RCS_DEBUG != 0
then
	set -x
	Q=""
	S="ls -lR; cat $TTY"
else
	Q="-q"
	S=""
fi
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
checkin $Q -u -tjunk.desc junk.txt
checkup junk.* >>$TTY
eval $S
#
cat <<eof/
**
**
**	Case 2.	Shows junk.txt (which is assumed to have changes), and suppress
**		junk.desc using the -x option.
eof/
checkout $Q -l junk.txt
touch junk.txt
checkup -x.desc junk.* >>$TTY
eval $S
#
#
cat <<eof/
**
**
**	Case 3.	Traverses the local tree, suppressing ".out" and ".desc" files.
**		Again, junk.txt is reported.
eof/
checkup -x.out -x.desc .. >>$TTY
eval $S
rm -f junk.* RCS/junk.*
cd ..
