: '$Header: /users/source/archives/cm_tools.vcs/src/checkout/src/RCS/rcsget.sh,v 4.0 1988/11/10 09:31:26 ste_cm Rel $'
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
	($0 -d $OPTS .)
else
	for i in $*
	do
		if [ -d $i ]
		then
			case $i in
			.|./*|..|../*)
				cd $i
				($0 $DOPT $OPTS *)
				cd $WD
				;;
			sccs|SCCS|.*|\$.*.\$)
				echo '?? ignored directory "'$i'"'
				;;
			$RCS)
				($0 $OPTS $RCS/*,v $RCS/.*,v)
				;;
			*)
				echo '** checkout directory "'$i'"'
				cd $i
				($0 $DOPT $OPTS *)
				cd $WD
				;;
			esac
		elif [ -z "$DOPT" ]
		then
			case $i in
			*\**)
				;;
			*)
				echo '** checkout '$OPTS $i
				checkout $OPTS $i
			esac
		fi
	done
fi
