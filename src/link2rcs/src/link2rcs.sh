: '@(#) $Header: /users/source/archives/cm_tools.vcs/src/link2rcs/src/RCS/link2rcs.sh,v 5.0 1989/03/28 13:49:24 ste_cm Rel $'
# Make a template of an existing tree, with symbolic links to the original
# tree's RCS directories (T.Dickey).
#
# Arguments are the subtrees to instantiate.  After the template is built,
# the 'rcsget' script can be run to extract files for the tree.
#
# $Log: link2rcs.sh,v $
# Revision 5.0  1989/03/28 13:49:24  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#
# Revision 4.0  89/03/28  13:49:24  ste_cm
# BASELINE Thu Aug 24 10:36:51 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.0  89/03/28  13:49:24  ste_cm
# BASELINE Mon Jun 19 14:44:56 EDT 1989
# 
# Revision 2.0  89/03/28  13:49:24  ste_cm
# BASELINE Thu Apr  6 13:37:00 EDT 1989
# 
# Revision 1.4  89/03/28  13:49:24  dickey
# updated getopt/usage for -m option
# 
# Revision 1.3  89/03/28  09:04:10  dickey
# added -m (merge) option.
# 

# hacks to make this run on apollo:
if [ -f /com/vt100 ]
then	PATH=$PATH:/sys5/bin
fi
TRACE=
#
WD=`pwd`
SRC=.
DST=
OPT=
set -$TRACE `getopt ms:d: $*`
for i in $*
do
	case $i in
	-m)	OPT=$i; shift;;
	-s)	SRC=$2;	shift; shift;;
	-d)	DST=$2;	shift; shift;;
	--)	shift; break;;
	-*)	echo "usage: `basename $0 .sh` [-m] [-s src_dir] [-d dst_dir] directories"
		exit 1;;
	esac
done
#
if [ -z "$DST" ]
then	echo '?? You must specify a destination-directory with "-d" option'
	exit 1
fi
if [ ! -d $SRC ]
then	echo '?? Argument given as source is not a directory: "'$SRC'"'
	exit 1
fi
if [ ! -d $DST ]
then	echo '?? Argument given as destination is not a directory: "'$DST'"'
	exit 1
fi
#
RCS=${RCS_DIR-RCS}
TMP=/tmp/link2rcs$$
cd $SRC;SRC=`pwd`
cd $WD
cd $DST;DST=`pwd`
echo SRC=$SRC
echo DST=$DST
#
trap "rm -f $TMP; exit 2" 1 2 3 15
for i in $*
do
	cd $SRC
	find $i -type d -print >$TMP
	cd $DST
	for j in `cat $TMP`
	do
		k=`basename $j`
		if test -r $j -o -s $j
		then
			if test "$OPT" = "-m"
			then
				echo '** merged:         '$j
				continue
			else
				echo '?? exists:         '$j
				break
			fi
		else
			case $k in
			.*|\$.*.\$)
				echo '?? ignored:        '$j
				;;
			$RCS)
				echo '** link-to-RCS:    '$j
				if ( ln -s $SRC/$j $j )
				then	continue
				else	break
				fi
				;;
			*)
				echo '** make directory: '$j
				if ( mkdir $j )
				then	continue
				else	break
				fi
			esac
		fi
	done
done
echo '** done **'
rm -f $TMP
