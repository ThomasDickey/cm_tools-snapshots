# $Id: Makefile,v 11.4 1992/11/24 07:43:58 dickey Exp $
# make-file for testing RCS checkout utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sh

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
run_tests:	$(SOURCES) $(ALL) ; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
