: '@(#)rcsget.sh	1.6 88/10/05 08:19:40'
# Check files out of RCS (T.E.Dickey)
#
# Use 'checkout' to checkout one or more files from the RCS-directory which is
# located in the current working directory, and then, to set the modification
# date of the checked-out files according to the last delta date (rather than
# the current date, as RCS assumes).
#
# Options are designed to feed-thru to 'co(1)'.  The "-d" option is designed
# to suppress redundant traversal of the tree, by directing this script to
# look only in the RCS-directories for filenames.
#
# Environment:
#	RCS_DIR - name of RCS-directory.  Note that if it is a relative path
#		(such as "../private"), this script will not work, though
#		'checkout' can cope.
#
RCS=${RCS_DIR-RCS}
TRACE=
#
WD=`pwd`
OPTS=
DOPT=
#
for i in $*
do	case $i in
	-[lpqrcswj]*)	OPTS="$OPTS $i";;
	-d)	DOPT=$i;;
	-*)	echo 'usage: <co_opts> -d -cCUTOFF files'
		exit 1;;
	*)	break;;
	esac
	shift
done
#
if [ -z "$1" ]
then
	$0 -d $OPTS .
elif [ "$1" != '*' ]
then
	for i in $*
	do
		if [ -d $i ]
		then
			if [ $i = $RCS ]
			then
				$0 $OPTS $RCS/*,v
			elif [ "$i" != sccs ]
			then
				echo '** checkout directory "'$i'"'
				cd $i
				$0 $DOPT $OPTS *
				cd $WD
			fi
		elif [ -z "$DOPT" ]
		then
			echo '** checkout '$OPTS $i
			checkout $OPTS $i
		fi
	done
fi
