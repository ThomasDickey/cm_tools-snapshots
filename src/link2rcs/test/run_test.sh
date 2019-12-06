#!/bin/sh
# $Id: run_test.sh,v 11.6 2019/12/03 01:19:21 tom Exp $
# test-script for link2rcs (RCS skeleton tree utility)

LANG=C; export LANG
LC_ALL=C; export LC_ALL
LC_TIME=C; export LC_TIME
LC_CTYPE=C; export LC_CTYPE
LANGUAGE=C; export LANGUAGE
LC_COLLATE=C; export LC_COLLATE
LC_NUMERIC=C; export LC_NUMERIC
LC_MESSAGES=C; export LC_MESSAGES

if test $# != 0
then
	echo '** '`date`
	PATH=`../../../support/testpath.sh`;	export PATH

	unset CDPATH
	HERE=`pwd`
	if [ -L RCS ]
	then
		rm -rf .@RCS
		mv RCS .@RCS
		mkdir RCS
		trap "unset CDPATH; cd $HERE; rmdir RCS; mv .@RCS RCS" EXIT INT QUIT TERM HUP
	elif [ ! -d RCS ]
	then
		mkdir RCS
		trap "unset CDPATH; cd $HERE; rmdir RCS" EXIT INT QUIT TERM HUP
	fi

	# initialize
	WD=`pwd`
	rm -rf subdir other
	mkdir subdir subdir/first subdir/RCS subdir/SCCS other
	cd ..
	rm -rf junk junk.*

	listtree.sh test >junk.old

	for name in $*
	do
		name=`basename $name .sh`
		$name.sh >$name.tst
		if test -f $WD/$name.ref
		then
			if ( cmp -s $name.tst $WD/$name.ref )
			then	echo '** ok:  '$name
				rm -f $name.tst
			elif ( cmp -s $name.tst $WD/$name.ref2 )
			then	echo "** no-SCCS:$name"
			else	echo '?? fail:'$name
				diff $name.tst $WD/$name.ref
			fi
		else	echo '** save:'$name
			mv $name.tst $WD/$name.ref
		fi
	done

	# cleanup
	rm -rf junk junk.*
	cd test
	rm -rf subdir other
else
	$0 case?.sh
fi
