: '$Header: /users/source/archives/cm_tools.vcs/src/checkin/test/RCS/run_test.sh,v 9.0 1989/03/30 14:49:43 ste_cm Rel $'
# test-script for RCS checkin-package.  Cannot really test setuid mode from
# a dumb script, but this does at least verify that the code is "sane".
#
# $Log: run_test.sh,v $
# Revision 9.0  1989/03/30 14:49:43  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
# Revision 8.0  89/03/30  14:49:43  ste_cm
# BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
# 
# Revision 7.0  89/03/30  14:49:43  ste_cm
# BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
# 
# Revision 6.0  89/03/30  14:49:43  ste_cm
# BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
# 
# Revision 5.0  89/03/30  14:49:43  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
# 
# Revision 4.0  89/03/30  14:49:43  ste_cm
# BASELINE Thu Aug 24 09:25:26 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.0  89/03/30  14:49:43  ste_cm
# BASELINE Mon Jun 19 13:09:53 EDT 1989
# 
# Revision 2.0  89/03/30  14:49:43  ste_cm
# BASELINE Thu Apr  6 09:17:23 EDT 1989
# 
# Revision 1.2  89/03/30  14:49:43  dickey
# corrected path for module-under-test
# 
# Revision 1.1  89/03/27  09:57:23  dickey
# Initial revision
# 
date
rm -rf junk
mkdir junk
cp Makefile junk/dummy
cd junk
PROG=../../bin/checkin
#
touch null_description

cat <<eof/
**
**	Show the original date for the new file:
eof/
ls -l dummy

cat <<eof/
**
**	Archive the file:
eof/
$PROG -u -tnull_description dummy

cat <<eof/
**
**	Show the resulting date for the new file:
eof/
ls -l dummy

cat <<eof/
**
**	Show the RCS log for the new file 'dummy':
eof/
rlog dummy
#
cd ..
rm -rf junk
