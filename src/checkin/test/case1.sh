#!/bin/sh
# $Id: case1.sh,v 11.1 1992/11/09 14:10:17 dickey Exp $
./admin.sh 0
sed -e s/@/\$/g >$WORK <<eof/
# @Id: dummy,v 10.1 92/02/10 15:30:51 tester Exp @
eof/
