#!/bin/sh
# $Id: case3.sh,v 11.0 1992/02/11 12:27:28 ste_cm Rel $
#
cat <<eof/
**	Checkins with baseline-file present
eof/

WORK=junk/dummy
#
rm -rf $WORK junk/$RCS_DIR
sed -e s/@/\$/g >$WORK <<eof/
# @Author: tester @
# @State: tested @
eof/
#
# make a fake baseline-file
(
	RCS_DIR=junk/$RCS_DIR;
	WORK=null_description
	mkdir $RCS_DIR;
	checkin -mbaseline -q -u2.1 -t$WORK $WORK
	mv $RCS_DIR/$WORK,v $RCS_DIR/RCS,v
)
