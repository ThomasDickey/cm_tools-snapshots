# $Header: /users/source/archives/cm_tools.vcs/bin/RCS/Makefile,v 3.0 1989/03/28 14:48:49 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 14:48:49  ste_cm
# BASELINE Mon Jun 19 12:55:50 EDT 1989
#
#	Revision 2.0  89/03/28  14:48:49  ste_cm
#	BASELINE Tue Apr  4 16:02:47 EDT 1989
#	
#	Revision 1.3  89/03/28  14:48:49  dickey
#	added 'rdiff' to install-list
#	
#	Revision 1.2  89/03/28  09:12:31  dickey
#	corrected 'clobber' rule
#	
#	Revision 1.1  89/03/27  13:34:18  dickey
#	RCS_BASE
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
destroy:	; rm -f *
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
