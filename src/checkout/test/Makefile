# $Id: Makefile,v 11.6 1994/11/09 00:16:26 tom Exp $
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
clean:				; -$(RM) $(CLEAN)
clobber:	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
run_test:	$(SOURCES) $(ALL) ; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
