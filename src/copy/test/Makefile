# $Id: Makefile,v 11.3 1993/04/27 10:37:08 dickey Exp $
# make-file for testing unix copy utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_test.sh

SOURCES	= Makefile $(SCRIPTS)

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; rm -f $(CLEAN)
clobber:	clean
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_test:	$(SOURCES)	; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
