#!/bin/sh
# $Id: listtree.sh,v 11.1 1992/11/24 15:51:31 dickey Exp $
#
for NAME in $*
do
	find $NAME \( -type d -o -type l -a -name RCS \) -exec ls -dF {} \; |\
		sort |\
		sed -e s+/RCS@\$+/RCS/+
done
