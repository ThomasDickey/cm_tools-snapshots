: '$Header: /users/source/archives/cm_tools.vcs/support/RCS/copy.sh,v 8.0 1989/03/28 09:50:30 ste_cm Rel $'
# cheap version of 'copy' utility, used for bootstrapping the make only.
S=
D=
for i in $*
do	i=$1
	case $i in
	-d)	shift
		D=$1
		S=`basename $D`
		;;
	-*)	shift;;
	*)	break;;
	esac
done
if test ! -n "$D"
then
	S=$1
	D=$2
fi
echo '** copy' $S to $D
#
# assume that $S is in the current directory...
P=`pwd`
tar cf /tmp/copy$$ $S
cd /tmp
tar xpf copy$$ $S
cd $P
mv /tmp/$S $D
cd /tmp
rm -f copy$$ $S
