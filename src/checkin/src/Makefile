# $Id: Makefile,v 11.6 1994/11/08 23:50:31 tom Exp $
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
install:	$(SOURCES) $B $(ALL)

clean:				; -$(RM) $(CLEAN)
clobber:	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/$(THIS):	$(THIS).o $(LIB_DEPS)	; $(LINK) -o $@ $(THIS).o $(LIB_ARGS)
$B/rcsput:	rcsput.o  $(LIB_DEPS)	; $(LINK) -o $@ rcsput.o  $(LIB_ARGS)

$(THIS).o \
rcsput.o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); $(LINT_EACH)
