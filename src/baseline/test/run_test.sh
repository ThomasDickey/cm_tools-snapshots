: '$Header: /users/source/archives/cm_tools.vcs/src/baseline/test/RCS/run_test.sh,v 2.0 1989/03/30 12:40:52 ste_cm Exp $'
# test-script for RCS baseline utility
#
# $Log: run_test.sh,v $
# Revision 2.0  1989/03/30 12:40:52  ste_cm
# BASELINE Tue Apr  4 16:08:39 EDT 1989
#
# Revision 1.3  89/03/30  12:40:52  dickey
# modified so this works if there is no RCS directory to copy from.
# 
# Revision 1.2  89/03/28  11:54:19  dickey
# made this work properly if RCS is a link (without permissions on target)
# 
# Revision 1.1  89/03/27  09:49:19  dickey
# Initial revision
# 
F="Makefile run_tests.sh"
date
#
rm -rf junk
mkdir junk junk/RCS
copy $F junk/
touch junk/not_baselined
if test -f RCS/Makefile,v
then
	copy RCS/* junk/RCS
	cd junk
else
	copy $F junk
	cd junk
	checkin -q -k -u -tnot_baselined $F
fi
ls -l
#
PROG=../../bin/baseline
chmod -w $F
rcs -e -q RCS/*,v
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
$PROG -$B $F
cat <<eof/
**
**	Case 2:	Now, baseline a file which was not archived (an error message
**		is generated).
eof/
touch not_baselined
$PROG -$B not_baselined
#
cd ..
rm -rf junk
