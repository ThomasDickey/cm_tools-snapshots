#!/bin/sh
# $Id: case3_d.sh,v 11.0 1992/02/11 11:30:19 ste_cm Rel $

WORK=junk/dummy
NAME=case3_d

cat <<eof/
**
**	$NAME) Add a new version to $WORK, no change
eof/
set_date -q -t1990/2/2 12:34:56 $WORK
checkin -q -mno-change -f -l4  -n$NAME $WORK
