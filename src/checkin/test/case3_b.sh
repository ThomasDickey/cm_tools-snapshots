#!/bin/sh
# $Id: case3_b.sh,v 11.0 1992/02/11 11:30:08 ste_cm Rel $

WORK=junk/dummy
NAME=case3_b

cat <<eof/
**
**	$NAME) Touch $WORK, no other change
eof/
set_date -q -t1989/2/2 12:34:56 $WORK
checkin -q -mnew-date -f -l -n$NAME $WORK
