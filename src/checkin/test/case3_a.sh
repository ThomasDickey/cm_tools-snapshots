#!/bin/sh
# $Id: case3_a.sh,v 11.0 1992/02/11 11:30:02 ste_cm Rel $

WORK=junk/dummy
NAME=case3_a

cat <<eof/
**
**	$NAME) Create archive from file $WORK, using current file-date
eof/
set_date -q -t1988/1/1 12:34:56 $WORK
checkin -q -tnull_description -u -l $WORK
