# $Id: Makefile,v 11.1 1992/08/05 14:37:10 dickey Exp $
# Make-file for RCS checkin utilities

####### (Environment) ##########################################################
THIS	= checkin
include ../../common.mk

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
run_tests:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(THIS).o $(LIBS)	; $(CC) -o $@ $(THIS).o $(LIBS)
$B/rcsput:	rcsput.o  $(LIBS)	; $(CC) -o $@ rcsput.o  $(LIBS)

$(THIS).o \
rcsput.o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); $(LINT_EACH)
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
