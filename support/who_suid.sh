#!/bin/csh -f
# $Id: who_suid.sh,v 11.1 1992/11/10 09:23:11 dickey Exp $
#
#	Scans the path for the first occurrence of the given program, tests to
#	see if it is running in set-uid mode.  If so, returns the owner.
#
set WHO=""
if $#argv then
	set NAME=$argv[1]
else
	set NAME=checkin
endif
#
foreach PP ( $path )
	set name=$PP/$NAME
	if ( -f $name && -x $name ) then
		set list=`ls -l $name`
		set mode=$list[1]
		set owner=$list[3]
		if ( "$owner" != "$USER" ) then
			set mode2=`echo $mode | sed -e s@s@@`
			if ( "$mode" != "$mode2" ) then
				set WHO=$owner
			endif
		endif
		break
	endif
end
echo $WHO
