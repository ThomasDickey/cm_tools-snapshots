#!/bin/sh
# $Id: case1_a.sh,v 11.1 1992/11/09 14:09:05 dickey Exp $

WORK=junk/dummy
NAME=case1_a

cat <<eof/
**
**	$NAME) Create archive from file $WORK, using existing keys
eof/
if test -f /com/vt100
then	W_OPT="-wtester"
else	W_OPT=""
fi
checkin -q -tnull_description -u -k $W_OPT -l $WORK
