: '@(#)rcsget.sh	1.5 88/08/17 10:06:20'
# Check files out of RCS (T.E.Dickey)
#
# Use 'checkout' to checkout one or more files from the RCS-directory which is
# located in the current working directory, and then, to set the modification
# date of the checked-out files according to the last delta date (rather than
# the current date, as RCS assumes).
#
# Options are designed to feed-thru to 'co(1)'.
#
TRACE=
#
WD=`pwd`
OPTS=
#
OPTS=
for i in $*
do	case $i in
	-[lpqrcswj]*)	OPTS="$OPTS $i";;
	-*)	echo 'usage: <co_opts> -cCUTOFF files'
		exit 1;;
	*)	break;;
	esac
	shift
done
#
if [ -z "$1" ]
then
	$0 $OPTS RCS/*,v
elif [ "$1" != '*' ]
then
	for i in $*
	do
		if [ -d $i ]
		then
			if [ $i = RCS ]
			then
				$0 $OPTS RCS/*,v
			elif [ "$i" != sccs ]
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
			then	checkout $OPTS $s
			else	echo '?? "'$i'" not found'
			fi
		fi
	done
fi
