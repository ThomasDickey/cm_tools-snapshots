#!/bin/sh
# $Id: case2.sh,v 11.2 1993/04/27 11:36:41 dickey Exp $
#
#	Case 2: Test the -m (merge) option by removing subdir-tree, and then
#		rerunning link2rcs.
#
rm -rf junk/test/subdir
link2rcs -m -d junk test | sed -e s@`pwd`@PWD@
#	
echo 'Resulting tree:'
listtree.sh junk | sed -e s+junk/++ -e /\^\$/d >junk.new
