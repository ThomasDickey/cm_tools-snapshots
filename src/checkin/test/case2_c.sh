#!/bin/sh
# $Id: case2_c.sh,v 11.0 1992/02/11 10:35:48 ste_cm Rel $

WORK=junk/dummy
NAME=case2_c

cat <<eof/
**
**	$NAME) Add a line to $WORK
eof/
echo 'new line' >>$WORK
set_date -q -t1990/2/2 12:34:56 $WORK
checkin -q -mnew-line -f -l  -n$NAME $WORK
