: '@(#)rcsget.sh	1.2 88/05/21 12:27:29'
# Check files out of RCS (T.E.Dickey)
#
# Use 'checkout' to checkout one or more files from the RCS-directory which is
# located in the current working directory, and then, to set the modification
# date of the checked-out files according to the last delta date (rather than
# the current date, as RCS assumes).
#
# Options are designed to feed-thru to 'co(1)'.
#
# Modified:
#
# Environment:
#	RUNLIB	- location of this & supporting code
#
# hacks to make this run on apollo:
SYS5=/sys5/bin
TRACE=
#
WD=`pwd`
BASE=${RUNLIB-//dickey/local/dickey/bin}/checkout
OPTS=
#
OPTS=
for i in $*
do	case $i in
	-[lpqrcswj])	OPTS="$OPTS $i";;
	-*)	echo 'usage: <ci_opts> -cCUTOFF files'
		exit 1;;
	*)	break;;
	esac
	shift
done
#
if [ -z "$1" ]
then	$0 $OPTS RCS
elif [ "$1" = RCS ]
then	$0 $OPTS RCS/*,v
else
	for i in $*
	do
		if [ -d $i ]
		then
			if [ "$i" != RCS -a "$i" != sccs ]
			then
				echo '** checkout directory "'$i'"'
				cd $i
				$0 $OPTS *
				cd $WD
			fi
		else
			s=`echo $i | sed 's/^RCS//'`
			s=`basename $s ,v`
			if [ "$s" = "$i" ]
			then	i="RCS/$i,v"
			fi
			echo '** checkout '$OPTS $i \($s\)
			if [ -f $i ]
			then	$BASE $OPTS $s
			else	echo '?? "'$i'" not found'
			fi
		fi
	done
fi
