: '$Header: /users/source/archives/cm_tools.vcs/src/checkout/test/RCS/run_test.sh,v 9.0 1989/03/30 14:52:36 ste_cm Rel $'
# test-script for RCS checkout utility
#
# $Log: run_test.sh,v $
# Revision 9.0  1989/03/30 14:52:36  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
# Revision 8.0  89/03/30  14:52:36  ste_cm
# BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
# 
# Revision 7.0  89/03/30  14:52:36  ste_cm
# BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
# 
# Revision 6.0  89/03/30  14:52:36  ste_cm
# BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
# 
# Revision 5.0  89/03/30  14:52:36  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
# 
# Revision 4.0  89/03/30  14:52:36  ste_cm
# BASELINE Thu Aug 24 09:29:10 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.0  89/03/30  14:52:36  ste_cm
# BASELINE Mon Jun 19 13:14:48 EDT 1989
# 
# Revision 2.0  89/03/30  14:52:36  ste_cm
# BASELINE Thu Apr  6 09:27:46 EDT 1989
# 
# Revision 1.2  89/03/30  14:52:36  dickey
# corrected path for module-under-test
# 
# Revision 1.1  89/03/27  10:07:16  dickey
# Initial revision
# 
date
rm -rf junk
mkdir  junk
cp Makefile junk/dummy
cd junk
PROG=../../bin/checkout
#
cat <<eof/
**
**	Creating a copy of 'Makefile', checked-in via -k (keys) option.
**	This will preserve the original checkin-date.
eof/
touch null_description
checkin -k -q -tnull_description dummy

cat <<eof/
**
**	Checking-out a copy of the resulting file 'dummy'
eof/
rm -f dummy

cat <<eof/
**
**	The resulting RCS log:
eof/
$PROG dummy
rlog dummy

cat <<eof/
**
**	The checked-out file:
eof/
ls -l dummy
#
cd ..
rm -rf junk
