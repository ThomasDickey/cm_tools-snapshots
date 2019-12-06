#!/bin/sh
# $Id: run_test.sh,v 11.4 2019/12/03 00:16:39 tom Exp $'
# run regression test for RCS permission utility
#
if test $# != 0
then
	echo '** '`date`
	PATH=`../../../support/testpath.sh checkin`; export PATH
	. testinit.sh
	Q="-q";		export Q
	RCS_DIR=FOO;	export RCS_DIR
	THISDIR=`pwd`

	for name in $*
	do
		name=`basename $name .sh`
		( $name.sh 2>&1 ) | \
			sed	-e s'@/tmp/permit[^ ]*@TMP@' \
				-e "s@$THISDIR@PWD@" \
				-e s@$ADMIN@ADMIN@g \
				-e s@$USER@USER@g >$name.tst
		if test -f $name.ref
		then
			if ( cmp -s $name.tst $name.ref )
			then	echo '** ok:  '$name
				rm -f $name.tst
			else	echo '?? fail:'$name
			fi
		else	echo '** save:'$name
			mv $name.tst $name.ref
		fi
	done

	rm -rf junk
else
	$0 case?.sh
fi
