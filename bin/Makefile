# $Id: Makefile,v 11.0 1992/02/04 10:01:51 ste_cm Rel $

####### (Development) ##########################################################
INSTALL_PATH=../../install_bin
B	= $(INSTALL_PATH)

####### (Standard Lists) #######################################################
IT	=\
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
	$B/vcs

####### (Standard Productions) #################################################
all:
clean:			; rm -f *.bak *.log *.out core
clobber:	clean	; rm -f [a-z]*
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:
install:	$(IT)
deinstall:	; rm -f $(IT)

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
$B/vcs:		vcs		; $(ADMIN)
