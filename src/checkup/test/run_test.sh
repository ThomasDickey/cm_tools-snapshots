: '$Header: /users/source/archives/cm_tools.vcs/src/checkup/test/RCS/run_test.sh,v 9.0 1989/03/28 11:25:33 ste_cm Rel $'
date
P=../../bin/checkup
rm -rf junk
mkdir junk
cd junk
rm -f junk.* RCS/junk.*
#
cp ../Makefile junk.txt
echo 'test file'>>junk.desc
#
cat <<eof/
**
**
**	Case 1.	Shows junk.desc (which is not checked-in).
eof/
checkin -q -u -tjunk.desc junk.txt
$P junk.* >>/dev/tty
#
cat <<eof/
**
**
**	Case 2.	Shows junk.txt (which is assumed to have changes), and suppress
**		junk.desc using the -x option.
eof/
checkout -q -l junk.txt
touch junk.txt
$P -x.desc junk.* >>/dev/tty
#
#
cat <<eof/
**
**
**	Case 3.	Traverses the local tree, suppressing ".out" and ".desc" files.
**		Again, junk.txt is reported.
eof/
$P -x.out -x.desc .. >>/dev/tty
rm -f junk.* RCS/junk.*
cd ..
rm -rf junk
