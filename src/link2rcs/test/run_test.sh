#!/bin/sh
# $Id: run_test.sh,v 11.1 1992/11/24 16:13:54 dickey Exp $
# test-script for link2rcs (RCS skeleton tree utility)
#
if test $# != 0
then
	echo '** '`date`
	PATH=`../../../support/test_path.sh`;	export PATH

	# initialize
	WD=`pwd`
	rm -rf subdir other
	mkdir subdir subdir/first subdir/RCS other
	cd ..
	rm -rf junk junk.*

	list_tree.sh test >junk.old

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
