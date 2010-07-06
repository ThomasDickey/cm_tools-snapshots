#!/bin/sh
# $Id: run_test.sh,v 11.4 2010/07/05 19:51:35 tom Exp $
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
