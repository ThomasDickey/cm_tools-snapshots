# $Id: Makefile,v 11.6 1994/11/08 23:54:55 tom Exp $
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
install:	$(SOURCES) $B $(ALL)

clean:				; -$(RM) $(CLEAN)
clobber: 	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
run_test:
sources: 	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/$(THIS):	$(THIS).o $(LIB_DEPS)	; $(LINK) -o $@ $(THIS).o $(LIB_ARGS)
$B/rcsget:	rcsget.o $(LIB_DEPS)	; $(LINK) -o $@ rcsget.o $(LIB_ARGS)

$(THIS).o \
rcsget.o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); $(LINT_EACH)
