#!/bin/sh
# $Id: case1_a.sh,v 11.0 1992/02/11 10:21:01 ste_cm Rel $

WORK=junk/dummy
NAME=case1_a

cat <<eof/
**
**	$NAME) Create archive from file $WORK, using existing keys
eof/
checkin -q -tnull_description -u -k -l $WORK
