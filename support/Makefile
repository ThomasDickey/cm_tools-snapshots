# $Id: Makefile,v 11.4 1994/11/09 00:16:43 tom Exp $

TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SCRIPTS	= \
	testpath.sh\
	testinit.sh\
	who_suid.sh

C_SRC	= \
	run_tool.c \
	set_date.c

SOURCES	= $(SCRIPTS)

ALL	= \
	run_tool \
	set_date

####### (Standard Productions) #################################################
all:	$(SOURCES) $(ALL)
clean:			; -$(RM) $(CLEAN)
clobber:	clean	; -$(RM) $(ALL)
destroy:		; $(DESTROY)
run_tests:
sources:	$(SOURCES)
install:
deinstall:

####### (Details of Productions) ###############################################
Z	= $(CC) $(CFLAGS) $(CPP_OPTS) -o $@ $@.c $(LIB_ARGS)
run_tool:	$(RCSDEFS_H) $(LIB_DEPS) ; $Z
set_date:	$(PTYPES_H)  $(LIB_DEPS) ; $Z

$(SOURCES):		; $(GET) $@
