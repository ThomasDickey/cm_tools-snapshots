#!/bin/csh -f
# $Id: clean.sh,v 11.1 1992/11/11 11:03:13 dickey Exp $
if $?SETUID then
	if ( -d junk/RCS ) $SETUID chmod 777 junk/RCS
endif
rm -rf junk null_description
