#!/bin/sh
# $Id: case3_f.sh,v 11.1 1992/11/12 07:34:09 dickey Exp $

WORK=junk/dummy
NAME=case3_f

cat <<eof/
**
**	$NAME) Add a new branch to $WORK, no change other than date
**		RCS version 4 fails because of an error in locking branches.
eof/
echo 'new-line' >>$WORK
set_date -q -t1990/3/4 12:34:56 $WORK
checkin -q -mmodify-branch -f -lcase3_c.3 -n$NAME $WORK
