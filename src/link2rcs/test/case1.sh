#!/bin/sh
# $Id: case1.sh,v 11.1 1992/11/24 16:13:04 dickey Exp $
#
#
#	Case 1: Test link2rcs by making a copy of the test-directory under junk
#
mkdir junk
link2rcs -d junk test 2>&1 | sed -e s@`pwd`@PWD@
#	
echo 'Resulting tree:'
list_tree.sh junk | sed -e s+\^junk/++ -e /\^\$/d
