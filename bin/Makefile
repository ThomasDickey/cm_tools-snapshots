# $Id: Makefile,v 9.0 1989/10/13 13:57:35 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 9.0  1989/10/13 13:57:35  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
#	Revision 8.0  89/10/13  13:57:35  ste_cm
#	BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
#	
#	Revision 7.0  89/10/13  13:57:35  ste_cm
#	BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
#	
#	Revision 6.0  89/10/13  13:57:35  ste_cm
#	BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
#	
#	Revision 5.0  89/10/13  13:57:35  ste_cm
#	BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#	
#	Revision 4.1  89/10/13  13:57:35  dickey
#	removed 'rdiff' from install-list
#	
#	Revision 4.0  89/08/22  08:23:21  ste_cm
#	BASELINE Thu Aug 24 09:14:19 EDT 1989 -- support:navi_011(rel2)
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
	$B/rcsput

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
