# $Id: Makefile,v 11.6 1994/11/09 00:14:26 tom Exp $
# MAKE-file for RCS skeleton tree utility

####### (Environment) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

THIS	= link2rcs

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)
CSRC	= $(THIS).c
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
$(THIS).o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
