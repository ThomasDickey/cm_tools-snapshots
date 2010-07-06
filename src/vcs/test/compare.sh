#!/bin/sh
# $Id: compare.sh,v 11.3 2010/07/05 20:23:55 tom Exp $
# test-script for RCS baseline utility
#
echo '** comparing results'

LANG=C; export LANG
LC_ALL=C; export LC_ALL
LC_TIME=C; export LC_TIME
LC_CTYPE=C; export LC_CTYPE
LANGUAGE=C; export LANGUAGE
LC_COLLATE=C; export LC_COLLATE
LC_NUMERIC=C; export LC_NUMERIC
LC_MESSAGES=C; export LC_MESSAGES

mv $1.tst $1.out
sed	-e s/${ADMIN-ADMIN}/{LOGNAME}/\
	-e s/${LOGNAME-LOGNAME}/{LOGNAME}/\
	-e 's/{{LOGNAME}}/{LOGNAME}/g'\
	-e s/${USER-LOGNAME}/{LOGNAME}/\
	-e 's@/tmp/permit[a-zA-Z0-9]*@{TEXT}@' $1.out >$1.tst
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
		diff $1.ref $1.tst 
	fi
else
	echo '** save:	'$1.ref
	mv $1.tst $1.ref
fi
