#!/bin/sh
# $Id: case3.sh,v 11.1 1992/11/09 08:04:33 dickey Exp $
#
cat <<eof/
**	Checkins with baseline-file present
eof/
./admin.sh 2
sed -e s/@/\$/g >$WORK <<eof/
# @Author: tester @
# @State: tested @
eof/
