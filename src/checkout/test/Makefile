# $Id: Makefile,v 11.5 1993/04/27 10:38:05 dickey Exp $
# make-file for testing RCS checkout utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_test.sh

SOURCES	= Makefile $(SCRIPTS)
ALL	=

####### (Standard Productions) #################################################
all:		$(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_test:	$(SOURCES) $(ALL) ; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
