#!/bin/sh
# $Id: run_test.sh,v 11.6 1992/11/24 08:25:44 dickey Exp $
#
#	Runs regression tests for 'checkin' and 'rcsput'
#
if test $# != 0
then
	echo '** '`date`
	PATH=`../../../support/test_path.sh`; export PATH
	. test_setup.sh

	SETUID=`whose_suid.sh checkin`
	if test -n "$SETUID"
	then	ADMIN=$SETUID
	fi
	export	ADMIN
	export	SETUID

	./clean.sh
	trap "./clean.sh" 0 1 2 5 15
	WORK=junk/dummy;export WORK

	mkdir junk
	touch null_description

	for N in $*
	do
		name=`basename $N .sh`
		if test ! -f $name.sh
		then	continue
		fi
		echo '**'
		echo '** testing '$name
		. $name.sh
		for NN in ${name}_*.sh
		do
			if test ! -f $NN
			then	break
			fi
			NAME=`basename $NN .sh`
			rm -f $NAME.out $NAME.log
			. $NN
			run_tool rlog $WORK | \
				sed	-e s@$ADMIN@ADMIN@g \
					-e s@$USER@USER@g \
				>$NAME.log
			if test -f $NAME.ref
			then
				if cmp -s $NAME.log $NAME.ref
				then
					echo '** ok   '$NAME
					rm -f $NAME.log
				else
					echo '?? diff '$NAME.log
					diff $NAME.log $NAME.ref
					cp junk/$RCS_DIR/`basename $WORK`,v $NAME.out
					cp junk/$WORK $NAME.in
					exit 1
				fi
			else
				echo '** save '$NAME.ref
				mv $NAME.log $NAME.ref
			fi
		done
	done
else
	$0 case?.sh
fi
