#!/bin/sh
# $Id: case2_b.sh,v 11.0 1992/02/11 10:38:55 ste_cm Rel $

WORK=junk/dummy
NAME=case2_b

cat <<eof/
**
**	$NAME) Touch $WORK, no other change
eof/
set_date -q -t1989/2/2 12:34:56 $WORK
checkin -q -mnew-date -f -l -n$NAME $WORK
