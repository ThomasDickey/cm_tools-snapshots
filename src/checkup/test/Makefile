# $Id: Makefile,v 11.2 1992/10/09 12:47:21 dickey Exp $
# make-file for testing RCS/sccs checkup utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sh

SOURCES	= Makefile $(SCRIPTS)

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; rm -f $(CLEAN)
clobber:	clean
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_tests:	$(SOURCES)	; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
