: '$Header: /users/source/archives/cm_tools.vcs/src/baseline/src/RCS/baseline.sh,v 2.0 1989/03/30 08:04:17 ste_cm Exp $'
# Baseline one or more files using the RCS utilities.  The file must have been
# archived before (or it will be ignored).  This script must be run from the
# directory containing the files to be archived.
#
# $Log: baseline.sh,v $
# Revision 2.0  1989/03/30 08:04:17  ste_cm
# BASELINE Tue Apr  4 16:06:13 EDT 1989
#
# Revision 1.4  89/03/30  08:04:17  dickey
# set state "Rel" (release) on baselined-files
# 
# Revision 1.3  89/03/29  11:28:19  dickey
# added "-v" (verbose) option.  Invoke 'permit' to keep its idea of the
# baseline-version up to date.
# 
Q="-q"
REV=2
DATE="BASELINE `date`"
for i in $*
do	i=$1
	case $i in
	-[0-9]*.*)
		echo '? you may specify only the whole-number version'
		exit 1
		;;
	-[0-9]*)
		REV=`echo $i | sed -e s/-//`
		echo '=>  '$REV
		shift
		;;
	-v)	Q=""
		shift
		;;
	-*)
		echo '? illegal option: expecting only baseline number'
		exit 1
		;;
	*)	break;
	esac
done
#
if ( permit -b$REV -p $Q )
then
	for i in $*
	do
		if test -f $i
		then
			echo '*** '$i
			if test -f RCS/$i,v
			then
				if ( checkout -l $Q $i )
				then
					checkin -r$REV.0 -f $Q -u -m"$DATE" -sRel $i
				else
					exit 4
				fi
			else	echo '? not archived: "'$i'"'
				exit 3
			fi
		else
			echo '??? '$i
		fi
	done
else
	exit 2
fi
