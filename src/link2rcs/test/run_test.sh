: '$Header: /users/source/archives/cm_tools.vcs/src/link2rcs/test/RCS/run_test.sh,v 5.0 1989/03/28 13:57:09 ste_cm Rel $'
# test-script for link2rcs (RCS skeleton tree utility)
#
# $Log: run_test.sh,v $
# Revision 5.0  1989/03/28 13:57:09  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#
# Revision 4.0  89/03/28  13:57:09  ste_cm
# BASELINE Thu Aug 24 10:38:03 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.0  89/03/28  13:57:09  ste_cm
# BASELINE Mon Jun 19 14:46:26 EDT 1989
# 
# Revision 2.0  89/03/28  13:57:09  ste_cm
# BASELINE Thu Apr  6 13:50:00 EDT 1989
# 
# Revision 1.3  89/03/28  13:57:09  dickey
# corrected test-cleanup
# 
# Revision 1.2  89/03/28  13:53:53  dickey
# rewrote, making this check for plausible errors
# 
# Revision 1.1  89/03/27  09:01:10  dickey
# Initial revision
# 
date
P=./bin/link2rcs

# initialize
rm -rf subdir other
mkdir subdir subdir/first subdir/RCS other
cd ..
rm -rf junk junk.*

find test -type d -exec ls -dF {} \; >junk.old

cat <<eof/
**
**	Case 1: Test link2rcs by making a copy of the test-directory under junk
**
eof/
mkdir junk
$P -d junk test

cat <<eof/
**	
**		resulting tree:
eof/
find junk -print
find junk \( -type d -o -type l \) -exec ls -dF {} \; |\
	sed -e s+junk/++ -e /\^\$/d >junk.new

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
$P -m -d junk test
cat <<eof/
**	
**		resulting tree:
eof/
find junk -print
find junk \( -type d -o -type l \) -exec ls -dF {} \; |\
	sed -e s+junk/++ -e /\^\$/d >junk.new

cat <<eof/
**
**		differences between trees (should be none):
eof/
diff junk.old junk.new

# cleanup
rm -rf junk junk.*
cd test
rm -rf subdir other
