#!/bin/sh
# $Id: run_test.sh,v 11.0 1991/10/11 13:27:09 ste_cm Rel $
# test-script for RCS baseline utility
#
F="Makefile run_tests.sh"
date
#
# run from test-versions:
rcstool=`which rcs`
PATH=:`pwd`:`cd ../bin;pwd`:`cd ../../../bin;pwd`:/bin:/usr/bin:/usr/ucb
export PATH
#
rm -rf junk
mkdir junk junk/RCS
copy $F junk/
touch junk/not_baselined
if test -f RCS/Makefile,v
then
	copy RCS/Makefile,v junk/RCS
	copy RCS/run_tests.sh,v junk/RCS
	cd junk
else
	copy $F junk
	cd junk
	checkin -q -k -u -tnot_baselined $F
fi
ls -l
#
chmod -w $F
$rcstool -e -q RCS/Makefile,v
#
touch not_baselined
N=`grep 'head[ ]*[0-9.]*;$' RCS/Makefile,v | sed -f ../run_tests.sed`
if test -n "$N"
then
	B=`expr $N + 1` 
else
	checkin -u -tnot_baselined Makefile
	B=2
fi

cat <<eof/
**
**	Case 1:	First, baseline copies of the following files (which are
**		known to be archived).  The baseline version is $B.0
eof/
baseline -$B $F
cat <<eof/
**
**	Case 2:	Now, baseline a file which was not archived (an error message
**		is generated).
eof/
touch not_baselined
baseline -$B not_baselined
#
cd ..
rm -rf junk
