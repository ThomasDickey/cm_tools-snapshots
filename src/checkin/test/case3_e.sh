#!/bin/sh
# $Id: case3_e.sh,v 11.0 1992/02/11 12:58:09 ste_cm Rel $

WORK=junk/dummy
NAME=case3_e

cat <<eof/
**
**	$NAME) Add a new branch to $WORK, no change other than date
eof/
rcs -q -u $WORK
#co  -q -f -lcase3_c $WORK
set_date -q -t1990/3/2 12:34:56 $WORK
ci -d -q -mnew-branch -f -lcase3_c.2 -n$NAME $WORK
ls -lR junk
#RCS_DEBUG=1 checkin -q -mnew-branch -f -ucase3_c.2 -n$NAME $WORK
