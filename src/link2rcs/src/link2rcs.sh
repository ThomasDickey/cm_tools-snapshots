: '@(#) $Header: /users/source/archives/cm_tools.vcs/src/link2rcs/src/RCS/link2rcs.sh,v 1.2 1988/11/10 15:59:12 dickey Exp $'
# Make a template of an existing tree, with symbolic links to the original
# tree's RCS directories (T.Dickey).
#
# Arguments are the subtrees to instantiate.  After the template is built,
# the 'rcsget' script can be run to extract files for the tree.
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
set -$TRACE `getopt s:d: $*`
for i in $*
do
	case $i in
	-s)	SRC=$2;	shift; shift;;
	-d)	DST=$2;	shift; shift;;
	--)	shift; break;;
	-*)	echo "usage: $0 [-s src_dir] [-d dst_dir] directories";exit 1;;
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
		if [ -r $j -o -s $j ]
		then	echo '?? exists:         '$j
			break
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
