#!/bin/sh
# $Id: run_test.sh,v 11.0 1991/10/18 11:27:32 ste_cm Rel $
# test-script for link2rcs (RCS skeleton tree utility)
#
date
#
# run from test-versions:
PATH=:`pwd`:`cd ../bin;pwd`:`cd ../../../bin;pwd`:/bin:/usr/bin:/usr/ucb
export PATH
#
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
link2rcs -d junk test

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
link2rcs -m -d junk test
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
