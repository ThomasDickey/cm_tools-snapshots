#!/bin/sh
# $Id: run_test.sh,v 11.3 1993/04/27 11:20:33 dickey Exp $
# test-script for RCS baseline utility
#
F="Makefile run_test.sh"
date
#
# run from test-versions:
for n in .. ../../.. ../../checkin ../../copy
do	PATH=`cd $n/bin;pwd`:$PATH
done
PATH=:`pwd`:$PATH export PATH
#
rm -rf junk
mkdir junk junk/RCS
copy $F junk/
touch junk/not_baselined
if test -f RCS/Makefile,v
then
	copy RCS/Makefile,v junk/RCS
	copy RCS/run_test.sh,v junk/RCS
	cd junk
else
	copy $F junk
	cd junk
	checkin -q -k -u -tnot_baselined $F
fi
ls -l
#
chmod -w $F
run_tool rcs -e -q RCS/Makefile,v
#
touch not_baselined
N=`grep 'head[ 	]*[0-9.]*;$' RCS/Makefile,v | sed -f ../run_test.sed`
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
