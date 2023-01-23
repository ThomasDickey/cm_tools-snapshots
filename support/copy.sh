#!/bin/sh
# $Id: copy.sh,v 11.1 1991/06/05 11:20:57 tom Exp $
# cheap version of 'copy' utility, used for bootstrapping the make only.
S=
D=
for i in $*
do	i=$1
	case $i in
	-d)	shift
		D=$1
		S=`basename $D`
		;;
	-*)	shift;;
	*)	break;;
	esac
done
if test ! -n "$D"
then
	S=$1
	D=$2
fi
echo '** copy' $S to $D
#
# assume that $S is in the current directory...
cp $S $D
