# $Header: /users/source/archives/cm_tools.vcs/bin/RCS/Makefile,v 4.0 1989/08/22 08:23:21 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 4.0  1989/08/22 08:23:21  ste_cm
# BASELINE Thu Aug 24 09:14:19 EDT 1989 -- support:navi_011(rel2)
#
#	Revision 3.1  89/08/22  08:23:21  dickey
#	corrected 'destroy' rule
#	
#	Revision 3.0  89/03/28  14:48:49  ste_cm
#	BASELINE Mon Jun 19 12:55:50 EDT 1989
#	

####### (Development) ##########################################################
INSTALL_PATH=/ste_site/ste/bin
B	= $(INSTALL_PATH)

####### (Standard Lists) #######################################################
ALL	=\
	$B/baseline\
	$B/checkin\
	$B/checkout\
	$B/checkup\
	$B/copy\
	$B/ded\
	$B/ded.hlp\
	$B/link2rcs\
	$B/permit\
	$B/rcsget\
	$B/rcsput\
	$B/rdiff

####### (Standard Productions) #################################################
all:		; @echo '** use "make install"'
clean:
clobber:	clean	; rm -f [a-z]*
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:
install:	$(ALL)
deinstall:	; rm -f $(ALL)

####### (Details of Productions) ###############################################
USER	= ./copy -d $@
ADMIN	= ./copy -s -d $@

$B/baseline:	baseline	; $(USER)
$B/checkin:	checkin		; $(ADMIN)
$B/checkout:	checkout	; $(ADMIN)
$B/checkup:	checkup		; $(USER)
$B/copy:	copy		; $(USER)
$B/ded:		ded		; $(USER)
$B/ded.hlp:	ded.hlp		; $(USER)
$B/link2rcs:	link2rcs	; $(USER)
$B/permit:	permit		; $(USER)
$B/rcsget:	rcsget		; $(USER)
$B/rcsput:	rcsput		; $(USER)
$B/rdiff:	rdiff		; $(USER)
