: '$Header: /users/source/archives/cm_tools.vcs/src/baseline/src/RCS/baseline.sh,v 4.0 1989/06/15 14:40:22 ste_cm Rel $'
# Baseline one or more files using the RCS utilities.  The file must have been
# archived before (or it will be ignored).  This script must be run from the
# directory containing the files to be archived.
#
# $Log: baseline.sh,v $
# Revision 4.0  1989/06/15 14:40:22  ste_cm
# BASELINE Thu Aug 24 09:20:00 EDT 1989 -- support:navi_011(rel2)
#
# Revision 3.0  89/06/15  14:40:22  ste_cm
# BASELINE Mon Jun 19 13:02:34 EDT 1989
# 
# Revision 2.2  89/06/15  14:40:22  dickey
# make -m argument identical for permit and checkin (requires new
# version of permit).
# 
# Revision 2.1  89/06/14  07:33:40  dickey
# added -m, -n, -r options as well as expanded usage-message
# 
# Revision 2.0  89/03/30  08:04:17  ste_cm
# BASELINE Tue Apr  4 16:06:13 EDT 1989
# 
Q="-q"
WD=`pwd`
RCS=${RCS_DIR-RCS}
REV=2
NOP=
OPTS=
RECURS=
MSG="-mBASELINE `date`"
#
trap "echo 'aborted ...'; exit 1" 1 2 3 15
#
for i in $*
do
    i=$1
    case $i in
    -m*) i=`echo $i | sed -e s/-m// -e s/\ /./g`
	OPTS="$OPTS -m$i"
	MSG="$MSG -- $i"
	shift
	;;
    -*)	OPTS="$OPTS $i"
	shift
	case $i in
	-[0-9]*.*)
		echo '? you may specify only the whole-number version'
		exit 1
		;;
	-[0-9]*)
		REV=`echo $i | sed -e s/-//`
		echo '=>  '$REV
		;;
	-n)	NOP=-n
		;;
	-r)	RECURS=$i
		SCRIPT=`which $0`
		case $SCRIPT in
		./*)	SCRIPT=$WD/$0
			;;
		esac
		;;
	-v)	Q=""
		;;
	-*)
		cat <<eof/
?? illegal option "$i"

Usage: baseline [options] files

Options are:
  -{integer}	baseline version (must be higher than last baseline)
  -m{text}	append reason for baseline to date-message
  -n		no-op (show what would be done, but don't do it)
  -r		recur when lower-level directories are found
  -v		verbose (shows details)
eof/
		exit 1
		;;
	esac
	;;
    *)	break;
    esac
done
#
if ( permit -b$REV -p $Q $NOP "$MSG" $RCS )
then
	for i in $*
	do
		if test -f $i
		then
			echo '*** '$i
			if test -f $RCS/$i,v
			then
				if test ".$NOP" != "."
				then
					continue
				elif ( checkout -l $Q $i )
				then
					checkin -r$REV.0 -f $Q -u "$MSG" -sRel $i
				else
					exit 4
				fi
			elif test ".$RECURS" = "."
			then	echo '? not archived: "'$i'"'
				exit 3
			fi
		elif test -d $i
		then
			if test $i != $RCS -a ".$RECURS" = ".-r"
			then
				echo '+++ recurring to '$i
				cd $i
				$SCRIPT $OPTS *
				cd $WD
				echo '--- returning to '$WD
			fi
		else
			echo '??? '$i
		fi
	done
else
	exit 2
fi
