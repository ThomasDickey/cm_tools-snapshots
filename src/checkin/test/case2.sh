#!/bin/sh
# $Id: case2.sh,v 11.1 1992/11/09 08:04:21 dickey Exp $
#
cat <<eof/
**	Checkins with no baseline-file present
eof/
./admin.sh 0
sed -e s/@/\$/g >$WORK <<eof/
# @Id: dummy,v 10.1 92/02/10 15:30:51 tester Exp @
eof/
