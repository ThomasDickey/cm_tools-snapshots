# $Id: Makefile,v 11.4 1993/05/06 13:51:01 dickey Exp $

THIS	= cm_tools
TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Development) ##########################################################
USE	= man
SECTION	= 1
PREFIX	= $(INSTALL_MAN)/$(USE)$(SECTION)/

####### (Standard Lists) #######################################################
IT	=\
	$(PREFIX)baseline.$(SECTION)\
	$(PREFIX)checkin.$(SECTION)\
	$(PREFIX)checkout.$(SECTION)\
	$(PREFIX)checkup.$(SECTION)\
	$(PREFIX)copy.$(SECTION)\
	$(PREFIX)link2rcs.$(SECTION)\
	$(PREFIX)permit.$(SECTION)\
	$(PREFIX)rcsget.$(SECTION)\
	$(PREFIX)rcsput.$(SECTION)\
	$(PREFIX)vcs.$(SECTION)

SOURCES	=\
	baseline.man\
	checkin.man\
	checkout.man\
	checkup.man\
	copy.man\
	link2rcs.man\
	permit.man\
	rcsget.man\
	rcsput.man\
	vcs.man

ALL	=

####### (Standard Productions) #################################################
all:		$(SOURCES) $(ALL)
clean:					; rm -f $(CLEAN)
clobber:	clean			; rm -f $(ALL) *.cat
destroy:	clobber			; $(DESTROY)
run_tests:
sources:	$(SOURCES)
install:	$(IT)
deinstall:				; rm -f $(IT)

####### (Details of Productions) ###############################################
$(PREFIX)baseline.$(SECTION):	baseline.$(USE)	; $(PUT)
$(PREFIX)checkin.$(SECTION):	checkin.$(USE)	; $(PUT)
$(PREFIX)checkout.$(SECTION):	checkout.$(USE)	; $(PUT)
$(PREFIX)checkup.$(SECTION):	checkup.$(USE)	; $(PUT)
$(PREFIX)copy.$(SECTION):	copy.$(USE)	; $(PUT)
$(PREFIX)link2rcs.$(SECTION):	link2rcs.$(USE)	; $(PUT)
$(PREFIX)permit.$(SECTION):	permit.$(USE)	; $(PUT)
$(PREFIX)rcsget.$(SECTION):	rcsget.$(USE)	; $(PUT)
$(PREFIX)rcsput.$(SECTION):	rcsput.$(USE)	; $(PUT)
$(PREFIX)vcs.$(SECTION):	vcs.$(USE)	; $(PUT)

$(SOURCES):				; $(GET) $@
