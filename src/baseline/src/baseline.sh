: '$Header: /users/source/archives/cm_tools.vcs/src/baseline/src/RCS/baseline.sh,v 1.2 1988/07/08 06:54:58 dickey Exp $'
# Baseline one or more files using the RCS utilities.  The file must have been
# archived before (or it will be ignored).  This script must be run from the
# directory containing the files to be archived.
REV=2
DATE="BASELINE `date`"
for i in $*
do
	case $i in
	-[0-9]*.*)
		echo '? you may specify only the whole-number version'
		exit 1
		;;
	-[0-9]*)
		REV=`echo $i | sed -e s/-//`
		echo '=>  '$REV
		;;
	-*)
		echo '? illegal option: expecting only baseline number'
		exit 1
		;;
	*)
		echo '*** '$i
		if [ -f RCS/$i,v ]
		then	checkout -l -q $i
			checkin -r$REV.0 -f -q -u -m"$DATE" $i
		else	echo '? not archived: "'$i'"'
			exit 2
		fi
		;;
	esac
done
