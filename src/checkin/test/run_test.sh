#!/bin/sh
# $Id: run_test.sh,v 11.0 1992/07/16 09:25:21 ste_cm Rel $
#
#	Runs regression tests for 'checkin' and 'rcsput'
#
if test $# != 0
then
	echo '** '`date`
	PATH=`cd ../bin;pwd`:`pwd`:$PATH; export PATH
	RCS_DIR=RCS;	export RCS_DIR
	RCS_BASE="";	export RCS_BASE
	RCS_COMMENT="";	export RCS_COMMENT

	rm -rf junk null_description
	trap "rm -rf junk null_description" 0 1 2 5 15
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
			rlog $WORK | sed -e s@`whoami`@USER@g >$NAME.log
			if test -f $NAME.ref
			then
				if cmp -s $NAME.log $NAME.ref
				then
					echo '** ok   '$NAME
					rm -f $NAME.log
				else
					echo '?? diff '$NAME.log
					mv junk/$RCS_DIR/`basename $WORK`,v $NAME.out
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
