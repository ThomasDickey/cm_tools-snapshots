# $Id: Makefile,v 11.7 1994/11/09 00:26:19 tom Exp $
# make-file for testing RCS skeleton tree utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_test.sed\
	run_test.sh

SOURCES	= Makefile $(SCRIPTS) run_tool.c
ALL	= run_tool

####### (Standard Productions) #################################################
all:		$(SOURCES) $(ALL)
clean:				; -$(RM) $(CLEAN)
clobber:	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
run_test:	$(SOURCES) $(ALL) ; $(RUN_TESTS)

####### (Details of Productions) ###############################################
run_tool:	run_tool.c $(PTYPES_H) $(RCSDEFS_H) $(LIB_DEPS)
	$(CC) -o $@ $(CFLAGS) $(CPP_OPTS) $@.c $(LIB_ARGS)

$(SOURCES):			; $(GET) $@
