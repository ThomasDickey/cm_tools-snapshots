#!/bin/sh
# $Id: case3_e.sh,v 11.1 1992/10/27 12:20:40 dickey Exp $

WORK=junk/dummy
NAME=case3_e

cat <<eof/
**
**	$NAME) Add a new branch to $WORK, no change other than date
eof/
./run_tool rcs -q -u $WORK
#co  -q -f -lcase3_c $WORK
set_date -q -t1990/3/2 12:34:56 $WORK
./run_tool ci -d -q -mnew-branch -f -lcase3_c.2 -n$NAME $WORK
ls -lR junk
#RCS_DEBUG=1 checkin -q -mnew-branch -f -ucase3_c.2 -n$NAME $WORK
