#!/bin/sh
# $Id: case3_e.sh,v 11.4 1992/11/24 08:30:19 dickey Exp $

WORK=junk/dummy
NAME=case3_e

cat <<eof/
**
**	$NAME) Add a new branch to $WORK, no change other than date
eof/
if test -n "$SETUID"; then $SETUID chmod 777 junk/RCS;fi
run_tool rcs -q -u $WORK
#co  -q -f -lcase3_c $WORK
set_date -q -t1990/3/2 12:34:56 $WORK
run_tool ci -d -q -mnew-branch -f -lcase3_c.2 -n$NAME $WORK
if test -n "$SETUID"; then $SETUID chmod 755 junk/RCS;fi
ls -lR junk
#RCS_DEBUG=1 checkin -q -mnew-branch -f -ucase3_c.2 -n$NAME $WORK
