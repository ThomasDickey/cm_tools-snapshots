: '$Header: /users/source/archives/cm_tools.vcs/src/checkin/src/RCS/rcsput.sh,v 4.0 1989/05/09 10:20:22 ste_cm Rel $'
# Check-in one or more modules to RCS (T.E.Dickey).
# This uses 'checkin' to maintain file modification dates as the checkin times.
# 
# Options:
#	-b (passed to 'diff' in display of differences)
#	-c (use 'cat' for paging diffs)
#	-d (suppress check-in process, do differences only)
#	-h (passed to 'diff' in display of differences)
#	-L (specify logfile for differences)
#
#	'ci' options are passed thru without change.
#
#	If a directory name is given, this script will recur into lower levels.
#	All options will be inherited in recursion.
#
# Environment:
#	PAGER	- may override to use in difference listing
#	RCS_DIR - name of rcs-directory
#
# hacks to make this run on Apollo:
if [ -f /com/vt100 ]
then	PATH=$PATH:/sys5/bin
fi
#
RCS=${RCS_DIR-RCS}
BLANKS=
SILENT=
NOP=
LOG=
#
WD=`pwd`
#
# Process options
#
OPTS=
for i in $*
do	case $i in
	-q*)	SILENT="-s"
		REV="$REV $i";;
	-[rfklumnNst]*)	REV="$REV $i";;
	-L*)	F=`echo "$i" | sed -e s/-L//`
		if [ -z "$F" ]
		then	F="logfile"
		fi
		D=`dirname $F`
		F=`basename $F`
		cd $D
		LOG=`pwd`/$F
		i="-L$LOG"
		cd $WD;;
	-[bh])	BLANKS="$BLANKS $i";;
	-c)	PAGER="cat";;
	-d)	NOP="T";;
	-*)	echo 'usage: <ci_opts> -b -c -d -h -Lfile files'
		exit 1;;
	*)	break;;
	esac
	shift
	OPTS="$OPTS $i"
done
#
# Process list of files
for i in $*
do
	ACT=
	cd $WD
	D=`dirname $i`
	F=`basename $i`
	if [ -n "$LOG" ]
	then	echo '*** '$WD/$i >>$LOG
	fi
	cd $D
	D=`pwd`
	i=$F
	if [ -f $i ]
	then
		if [ ! -d $RCS -a -z "$NOP" ]
		then	mkdir $RCS
		fi
	elif [ -d $i ]
	then
		if [ $i != RCS -a $i != $RCS ]
		then	echo '*** recurring to '$i
			cd $i
			$0 $OPTS *
		fi
		continue
	else
		echo '*** Ignored "'$i'" (not a file)'
		continue
	fi
	j=$RCS/$i,v
	if [ -f $j ]
	then
		echo '*** Checking differences for "'$i'"'
		rm -f /tmp/diff[SE]$$
		rcsdiff $BLANKS $i >/tmp/diffS$$ 2>/tmp/diffE$$
		cat /tmp/diffE$$ 1>&2
		if [ -s /tmp/diffS$$ ]
		then
			if [ -z "$SILENT" ]
			then
				${PAGER-'more'} /tmp/diffS$$
			fi
			if [ -n "$LOG" ]
			then
				echo appending to logfile
				cat /tmp/diffS$$ >>$LOG
			fi
			ACT="D"
		else
			ERR=`fgrep 'no revisions present' /tmp/diffE$$`
			if [ -z "$ERR" ]
			then	echo '*** no differences found ***'
			else	ACT="I"
			fi
		fi
		rm -f /tmp/diff[SE]$$
	elif [ -f $i ]
	then
		if (file $i | fgrep -v packed | grep text$)
		then	ACT="I"
		else	echo '*** "'$i'" does not seem to be a text file'
		fi
	fi
#
	if [ -z "$NOP" -a -n "$ACT" ]
	then
		if [ "$ACT" = "I" ]
		then	echo '*** Initial RCS insertion of "'$i'"'
		else	echo '*** Applying RCS delta to "'$i'"'
		fi
		checkin $REV $i
	elif [ -n "$ACT" ]
	then
		if [ "$ACT" = "I" ]
		then	echo '--- This would be initial for "'$i'"'
		else	echo '--- Delta would be applied to "'$i'"'
		fi
	fi
done
