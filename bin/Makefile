# $Id: Makefile,v 11.5 1994/11/09 00:15:59 tom Exp $

####### (Development) ##########################################################
TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

B	= $(INSTALL_BIN)

####### (Standard Lists) #######################################################
IT	=\
	$B/baseline\
	$B/checkin\
	$B/checkout\
	$B/checkup\
	$B/copy\
	$B/link2rcs\
	$B/permit\
	$B/rcsget\
	$B/rcsput\
	$B/vcs

SOURCES	=\
	Makefile

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; -$(RM) $(CLEAN)
clobber:	clean		; -$(RM) $(ALL) [a-z]*
destroy:			; $(DESTROY)
run_test:
sources:
install:	$(IT)
deinstall:			; -$(RM) $(IT)

####### (Details of Productions) ###############################################
USER	= ./copy -d $@
ADMIN	= ./copy -s -d $@

$B/baseline:	baseline	; $(USER)
$B/checkin:	checkin		; $(ADMIN)
$B/checkout:	checkout	; $(ADMIN)
$B/checkup:	checkup		; $(USER)
$B/copy:	copy		; $(USER)
$B/link2rcs:	link2rcs	; $(USER)
$B/permit:	permit		; $(USER)
$B/rcsget:	rcsget		; $(USER)
$B/rcsput:	rcsput		; $(USER)
$B/vcs:		vcs		; $(ADMIN)
