#!/bin/sh
# $Id: case1.sh,v 11.2 1993/04/27 11:35:51 dickey Exp $
#
#
#	Case 1: Test link2rcs by making a copy of the test-directory under junk
#
mkdir junk
link2rcs -d junk test 2>&1 | sed -e s@`pwd`@PWD@
#	
echo 'Resulting tree:'
listtree.sh junk | sed -e s+\^junk/++ -e /\^\$/d
