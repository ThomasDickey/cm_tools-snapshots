# $Id: Makefile,v 11.3 1993/04/27 10:47:12 dickey Exp $

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
clean:			; rm -f $(CLEAN)
clobber:	clean	; rm -f $(ALL)
destroy:		; $(DESTROY)
run_tests:
sources:	$(SOURCES)
install:
deinstall:

####### (Details of Productions) ###############################################
Z	= $(CC) $(CFLAGS) $(CPP_OPTS) -o $@ $@.c $(LIBS)
run_tool:	$(RCSDEFS_H) $(LIBS) ; $Z
set_date:	$(PTYPES_H)  $(LIBS) ; $Z

$(SOURCES):		; $(GET) $@
