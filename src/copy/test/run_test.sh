: '$Header: /users/source/archives/cm_tools.vcs/src/copy/test/RCS/run_test.sh,v 9.0 1989/03/30 15:11:17 ste_cm Rel $'
# test unix copy utility be copying some files and directories; should have
# no differences.
#
# $Log: run_test.sh,v $
# Revision 9.0  1989/03/30 15:11:17  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
# Revision 8.0  89/03/30  15:11:17  ste_cm
# BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
# 
# Revision 7.0  89/03/30  15:11:17  ste_cm
# BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
# 
# Revision 6.0  89/03/30  15:11:17  ste_cm
# BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
# 
# Revision 5.0  89/03/30  15:11:17  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
# 
# Revision 4.0  89/03/30  15:11:17  ste_cm
# BASELINE Thu Aug 24 10:17:44 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.0  89/03/30  15:11:17  ste_cm
# BASELINE Mon Jun 19 14:19:05 EDT 1989
# 
# Revision 2.0  89/03/30  15:11:17  ste_cm
# BASELINE Thu Apr  6 13:10:28 EDT 1989
# 
# Revision 1.2  89/03/30  15:11:17  dickey
# tee-normal test-results so we look like other CM_TOOLS test-scripts
# 
# Revision 1.1  89/03/24  13:27:26  dickey
# Initial revision
# 
C=../bin/copy
L=run_tests.out
#
date | tee -a $L
rm -rf junk pile
mkdir junk junk/subdir1 junk/subdir2 junk/subdir1/nuisance
cp Makefile junk/first
$C Makefile junk/second
$C junk pile
ls -lR junk | sed -e s/junk/NAME/ >junk.out
ls -lR pile | sed -e s/pile/NAME/ >pile.out
if ( cmp -s junk.out pile.out )
then
	echo '** normal test' | tee -a $L
else
	echo '?? difference found' | tee -a $L
	diff junk.out pile.out | tee -a $L
fi
#
rm -rf junk pile junk.out pile.out
