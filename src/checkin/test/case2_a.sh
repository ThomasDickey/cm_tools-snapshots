#!/bin/sh
# $Id: case2_a.sh,v 11.0 1992/02/11 10:30:01 ste_cm Rel $

WORK=junk/dummy
NAME=case2_a

cat <<eof/
**
**	$NAME) Create archive from file $WORK, using current file-date
eof/
set_date -q -t1988/1/1 12:34:56 $WORK
checkin -q -tnull_description -u -l $WORK
