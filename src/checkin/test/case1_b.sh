#!/bin/sh
# $Id: case1_b.sh,v 11.0 1992/02/11 11:20:28 ste_cm Rel $

WORK=junk/dummy
NAME=case1_b

cat <<eof/
**
**	$NAME) Modify file $WORK, check-in with different keys
eof/
echo 'a new line' >>$WORK
set_date -q -t 92/2/11 10:00:00 $WORK
echo 'added line' | checkin -q -u14 -nSecond -sState $WORK
