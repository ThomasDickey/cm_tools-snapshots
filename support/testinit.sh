#!/bin/sh
# $Id: testinit.sh,v 11.2 1992/11/24 08:27:15 dickey Exp $
#
#	"Source" this to setup vcs-related tests
#
RCS_DIR=RCS;		export RCS_DIR
RCS_DEBUG=0;		export RCS_DEBUG
RCS_BASE="";		export RCS_BASE
RCS_COMMENT="";		export RCS_COMMENT

USER=`whoami`;		export USER
ADMIN=ADMIN
