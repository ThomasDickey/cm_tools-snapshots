: '$Header: /users/source/archives/cm_tools.vcs/src/permit/test/RCS/run_test.sh,v 9.0 1989/03/31 08:40:39 ste_cm Rel $'
# run regression test for RCS permission utility
#
# $Log: run_test.sh,v $
# Revision 9.0  1989/03/31 08:40:39  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
# Revision 8.0  89/03/31  08:40:39  ste_cm
# BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
# 
# Revision 7.0  89/03/31  08:40:39  ste_cm
# BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
# 
# Revision 6.0  89/03/31  08:40:39  ste_cm
# BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
# 
# Revision 5.0  89/03/31  08:40:39  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
# 
# Revision 4.0  89/03/31  08:40:39  ste_cm
# BASELINE Thu Aug 24 10:41:43 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.0  89/03/31  08:40:39  ste_cm
# BASELINE Mon Jun 19 14:50:57 EDT 1989
# 
# Revision 2.0  89/03/31  08:40:39  ste_cm
# BASELINE Thu Apr  6 13:59:39 EDT 1989
# 
# Revision 1.2  89/03/31  08:40:39  dickey
# added extra (preamble) test in case we have no RCS directory already.
# 
# Revision 1.1  89/03/23  14:01:43  dickey
# Initial revision
# 

date
PROG="../bin/permit"
if test -z "$LOGNAME"
then
	LOGNAME=`set - \`ls -ld .\`;echo $3`
fi

cat <<eof/
**
**	Creating a subdirectory, 'junk' which has one archived file, 'dummy'
**	The file-owner, "$LOGNAME" is the only one on the access list.  If this
**	test is run with the RCS directories (or symbolic links) intact for
**	the current directory, those names will be shown in the subsequent
**	cases.
eof/
rm -rf junk
mkdir junk junk/RCS
cp Makefile junk/dummy
cd junk
touch null
ci -q -tnull dummy
rcs -a$LOGNAME dummy
cd ..
$PROG junk

cat <<eof/
**
**	Review the test-directory just created; there should be no actions
**	needed for it:
eof/
$PROG -n junk

cat <<eof/
**
**	Case 1:	Shows the current 'permit' state of the local directory.
**		If 'permit' has not been run (unlikely) then it would invoke
**		'rcs' to tidy up.
eof/
$PROG -n

cat <<eof/
**
**	Case 2:	Shows the current 'permit' state of the parent directory.
eof/
$PROG -n ..

cat <<eof/
**
**	Case 3:	Shows what would be done to purge permissions from the
**		current directory.
eof/
$PROG -np

cat <<eof/
**
**	Case 4:	Shows what would be done to purge permissions on the parent
**		directory (including this one).
eof/
$PROG -np ..

cat <<eof/
**
**	Case 5:	Shows what would be done to purge your permissions from
**		this directory.  Assumes that "$LOGNAME" is your userid.
eof/
$PROG -ne$LOGNAME

rm -rf junk
