#!/bin/sh
# $Id: case3_c.sh,v 11.0 1992/02/11 11:30:14 ste_cm Rel $

WORK=junk/dummy
NAME=case3_c

cat <<eof/
**
**	$NAME) Add a line to $WORK
eof/
echo 'new line' >>$WORK
set_date -q -t1990/2/2 12:34:56 $WORK
checkin -q -mnew-line -f -l  -n$NAME $WORK
