# $Id: Makefile,v 11.2 1992/10/16 09:31:36 dickey Exp $

####### (Development) ##########################################################
TOP	= ../..
include $(TOP)/cm_library/support/cm_library.mk

B	= $(INSTALL_BIN)

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

SOURCES	=\
	Makefile

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL) [a-z]*
destroy:			; $(DESTROY)
run_tests:
sources:
install:	$(IT)
deinstall:			; rm -f $(IT)

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
