# $Id: Makefile,v 11.1 1992/11/24 08:21:57 dickey Exp $

TOP	= ../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
SCRIPTS	= \
	test_path.sh\
	test_setup.sh\
	whose_suid.sh

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
