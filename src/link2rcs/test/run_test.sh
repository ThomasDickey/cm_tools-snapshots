#!/bin/sh
# $Id: run_test.sh,v 11.3 1999/06/27 18:43:45 tom Exp $
# test-script for link2rcs (RCS skeleton tree utility)
#
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
