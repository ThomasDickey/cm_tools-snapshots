#!/bin/sh
# $Id: compare.sh,v 11.1 1992/11/23 12:06:26 dickey Exp $
# test-script for RCS baseline utility
#
echo '** comparing results'
mv $1.tst $1.out
sed	-e s/${ADMIN-ADMIN}/{LOGNAME}/\
	-e s/${LOGNAME-LOGNAME}/{LOGNAME}/\
	-e s/${USER-LOGNAME}/{LOGNAME}/\
	-e 's@/tmp/permit[a-z0-9]*@{TEXT}@' $1.out >$1.tst
rm -f $1.out
#
if test -f $1.ref
then
	if cmp -s $1.tst $1.ref
	then
		echo '** same:	'$1.ref
		rm -f $1.tst
	else
		echo '?? diff:	'$1.ref
		diff $1.tst $1.ref
	fi
else
	echo '** save:	'$1.ref
	mv $1.tst $1.ref
fi
