#!/bin/sh
# $Id: run_test.sh,v 9.0 1990/08/14 08:55:16 ste_cm Rel $
# test-script for RCS baseline utility
#
# $Log: run_test.sh,v $
# Revision 9.0  1990/08/14 08:55:16  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
# Revision 8.0  90/08/14  08:55:16  ste_cm
# BASELINE Tue Aug 14 09:45:20 1990 -- LINCNT, ADA_TRANS
# 
# Revision 7.1  90/08/14  08:55:16  dickey
# bypass bug in Apollo Bourne-shell
# 
# Revision 8.0  89/06/16  09:50:29  ste_cm
# BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
# 
# Revision 7.0  89/06/16  09:50:29  ste_cm
# BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
# 
# Revision 6.0  89/06/16  09:50:29  ste_cm
# BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
# 
# Revision 5.0  89/06/16  09:50:29  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
# 
# Revision 4.0  89/06/16  09:50:29  ste_cm
# BASELINE Thu Aug 24 09:21:12 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.0  89/06/16  09:50:29  ste_cm
# BASELINE Mon Jun 19 13:04:14 EDT 1989
# 
# Revision 2.1  89/06/16  09:50:29  dickey
# revise PATH to run from test-versions before installed versions.
# 
# Revision 2.0  89/03/30  12:40:52  ste_cm
# BASELINE Tue Apr  4 16:08:39 EDT 1989
# 
F="Makefile run_tests.sh"
date
#
# run from test-versions:
PATH=`echo $PATH | sed -e s/\^[\^:]\*:/:/`
PATH=:`pwd`:`cd ../bin;pwd`:`cd ../../../bin;pwd`$PATH
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
