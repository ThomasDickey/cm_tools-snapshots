#!/bin/sh
# $Id: run_test.sh,v 9.0 1989/12/08 09:18:05 ste_cm Rel $
# test-script for link2rcs (RCS skeleton tree utility)
#
# $Log: run_test.sh,v $
# Revision 9.0  1989/12/08 09:18:05  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
# Revision 8.0  89/12/08  09:18:05  ste_cm
# BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
# 
# Revision 7.0  89/12/08  09:18:05  ste_cm
# BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
# 
# Revision 6.0  89/12/08  09:18:05  ste_cm
# BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
# 
# Revision 5.3  89/12/08  09:18:05  dickey
# "ls -dF" produces inconsistent results (edited to cover up)
# 
# Revision 5.2  89/12/06  14:08:41  dickey
# sort result of 'find', since we cannot rely on directory-order
# 
# Revision 5.1  89/12/06  13:39:25  dickey
# template (test-directory) may contain symbolic links to RCS-directories.
# edit junk.old to account for this.
# 
# Revision 5.0  89/03/28  13:57:09  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
# 
date
PROG=./bin/link2rcs

# initialize
rm -rf subdir other
mkdir subdir subdir/first subdir/RCS other
cd ..
rm -rf junk junk.*

find test \( -type d -o -type l -a -name RCS \) -exec ls -dF {} \; | sort |\
	sed -e s+/RCS@\$+/RCS/+ >junk.old

cat <<eof/
**
**	Case 1: Test link2rcs by making a copy of the test-directory under junk
**
eof/
mkdir junk
$PROG -d junk test

cat <<eof/
**	
**		resulting tree:
eof/
find junk -print
find junk \( -type d -o -type l \) -exec ls -dF {} \; | sort |\
	sed -e s+RCS@\$+RCS/+ -e s+junk/++ -e /\^\$/d >junk.new

cat <<eof/
**
**		differences between trees (should be none):
eof/
diff junk.old junk.new

cat <<eof/
**
**	Case 2: Test the -m (merge) option by removing subdir-tree, and then
**		rerunning link2rcs.
eof/
rm -rf junk/test/subdir
$PROG -m -d junk test
cat <<eof/
**	
**		resulting tree:
eof/
find junk -print
find junk \( -type d -o -type l \) -exec ls -dF {} \; | sort |\
	sed -e s+RCS@\$+RCS/+ -e s+junk/++ -e /\^\$/d >junk.new

cat <<eof/
**
**		differences between trees (should be none):
eof/
diff junk.old junk.new

# cleanup
rm -rf junk junk.*
cd test
rm -rf subdir other
