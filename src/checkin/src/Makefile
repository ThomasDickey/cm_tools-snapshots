# $Id: Makefile,v 11.5 1993/09/22 13:46:22 dickey Exp $
# Make-file for RCS checkin utilities

THIS	= checkin
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)   $B/rcsput
CSRC	=    $(THIS).c    rcsput.c
SOURCES	= $(CSRC)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)

clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(THIS).o $(LIBS)	; $(LINK) -o $@ $(THIS).o $(LIBS)
$B/rcsput:	rcsput.o  $(LIBS)	; $(LINK) -o $@ rcsput.o  $(LIBS)

$(THIS).o \
rcsput.o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); $(LINT_EACH)
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
