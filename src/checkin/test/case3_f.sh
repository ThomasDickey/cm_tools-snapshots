#!/bin/sh
# $Id: case3_f.sh,v 11.0 1992/02/11 12:38:10 ste_cm Rel $

WORK=junk/dummy
NAME=case3_f

cat <<eof/
**
**	$NAME) Add a new branch to $WORK, no change other than date
eof/
echo 'new-line' >>$WORK
set_date -q -t1990/3/4 12:34:56 $WORK
checkin -q -mmodify-branch -f -lcase3_c.3 -n$NAME $WORK
