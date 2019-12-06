#!/bin/sh
# $Id: testinit.sh,v 11.3 2019/12/02 23:49:29 tom Exp $
#
#	"Source" this to setup vcs-related tests
#
unset LANG
unset LC_ALL
unset LC_CTYPE

RCS_DIR=RCS;		export RCS_DIR
RCS_DEBUG=0;		export RCS_DEBUG
RCS_BASE="";		export RCS_BASE
RCS_COMMENT="";		export RCS_COMMENT

USER=`whoami`;		export USER
ADMIN=ADMIN
