#!/bin/sh
# $Id: admin.sh,v 11.3 1992/11/10 09:30:15 dickey Exp $
#
# Reset the test-RCS directory to an initial-state at the beginning of a test
# cycle.
#
BASE=${1-0}
#
WD=`pwd`
WORK=junk/dummy
RCS_DIR=junk/RCS
rm -f $WORK
#
if test -d $RCS_DIR
then
	ls -ld $RCS_DIR >/tmp/admin$$
	if read mode links owner etc
	then
		rm -f /tmp/admin$$
		if test .$SETUID = .$owner
		then	$SETUID chacl -u $USER $RCS_DIR
		fi
		rm -rf $RCS_DIR
	fi </tmp/admin$$
fi
#
mkdir $RCS_DIR
if test $BASE != 0
then
	TEXT=null_description
	TEMP=/tmp/$TEXT
	if test .$ADMIN = .$SETUID -a -f /com/vt100
	then
		chacl -g spe $RCS_DIR;
		chacl -u $ADMIN $RCS_DIR
	fi
	cd $RCS_DIR
	$SETUID cp $WD/$TEXT /tmp/
	$SETUID ci -q -mbaseline -r$BASE.1 -t$TEMP $TEMP ./$TEXT,v
	$SETUID rcs -q -a$ADMIN,$USER ./$TEXT,v
	$SETUID mv ./$TEXT,v ./RCS,v
	cd $WD
fi
