# $Id: Makefile,v 11.4 1993/04/27 10:38:05 dickey Exp $
# Make-file for RCS checkout utilities

THIS	= checkout
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)   $B/rcsget
CSRC	=    $(THIS).c    rcsget.c
SOURCES	= Makefile $(CSRC)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)

clean:				; rm -f $(CLEAN)
clobber: 	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_test:
sources: 	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(THIS).o $(LIBS)	; $(CC) -o $@ $(THIS).o $(LIBS)
$B/rcsget:	rcsget.o $(LIBS)	; $(CC) -o $@ rcsget.o $(LIBS)

$(THIS).o \
rcsget.o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); $(LINT_EACH)
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
