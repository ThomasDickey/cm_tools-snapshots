#!/bin/sh
# $Id: run_test.sh,v 11.10 2010/07/05 20:48:14 tom Exp $
#
#	test-script for 'vcs' utility
#
if test $# != 0
then
	echo '** '`date`
	PATH=`../../../support/testpath.sh checkin permit`;	export PATH
	. ../../../support/testinit.sh
	RCS_DIR=FOO; export RCS_DIR

	SETUID=`who_suid.sh vcs`
	if test -n "$SETUID"
	then	SETUID2=`who_suid.sh checkin`
		if test "$SETUID" != "$SETUID2"
		then	echo '? both vcs and checkin must be same setuid-sense'
			SETUID=""
		else	ADMIN=$SETUID
		fi
	fi
	export	ADMIN
	export	SETUID

	WD=`pwd`
	JUNK='foo* FOO'
	TTY=/tmp/test$$
	TMP=/tmp/TEST$$
	CLEANUP="$SETUID rm -rf $TMP; rm -f $TTY"

	echo '** initializing tests'
	trap "$CLEANUP" 0 1 2 5 15
	Q="-q"
	$CLEANUP
	umask 000
	$SETUID mkdir $TMP
	umask 022
	cd $TMP
	$SETUID mkdir FOO
	$SETUID permit $Q -a$USER,$ADMIN -b2 .

	for name in $*
	do
		name=`basename $name .ref`
		echo '** Testing '`echo $name |tr '[a-z]' '[A-Z]'`
		LOG=$WD/$name.tst
		rm -f $LOG
		cd $TMP
		case $name in
		insert)
			(
				vcs -i foo/src foo/test foo2 2>&1
				showtree.sh $JUNK
			) >>$LOG
			;;
		unlock)
			(
				cp $WD/compare.sh foobar
				checkin $Q -l -t$WD/makefile.in foobar 2>$TTY
				vcs -u foobar 2>&1
			) >>$LOG
			;;
		delete)
			(
				vcs -d foo/src foo2 foo 2>&1
				showtree.sh $JUNK
			) >>$LOG
			;;
		esac
		cd $WD
		./compare.sh $name
	done
else
	$0 insert unlock delete
fi
