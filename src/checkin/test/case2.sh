#!/bin/sh
# $Id: case2.sh,v 11.0 1992/02/11 12:27:11 ste_cm Rel $
#
cat <<eof/
**	Checkins with no baseline-file present
eof/

WORK=junk/dummy
#
rm -rf $WORK junk/$RCS_DIR
sed -e s/@/\$/g >$WORK <<eof/
# @Id: dummy,v 10.1 92/02/10 15:30:51 tester Exp @
eof/
