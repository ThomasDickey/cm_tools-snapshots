# $Id: Makefile,v 11.4 1992/10/28 12:10:31 dickey Exp $
# make-file for testing RCS skeleton tree utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sed\
	run_tests.sh

SOURCES	= Makefile $(SCRIPTS) run_tool.c
ALL	= run_tool

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
run_tool:	run_tool.c $(PTYPES_H) $(RCSDEFS_H) $(LIBS)
	$(CC) -o $@ $(CFLAGS) $(CPP_OPTS) $@.c $(LIBS)

$(SOURCES):			; $(GET) $@
