# $Id: Makefile,v 11.4 1993/04/27 10:38:04 dickey Exp $
# MAKE-file for RCS skeleton tree utility

####### (Environment) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

THIS	= link2rcs

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)
CSRC	= $(THIS).c
SOURCES	= Makefile $(CSRC)
MAIN_O	= $(THIS).o $(LIBS)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)

clean:				; rm -f $(CLEAN)
clobber: 	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_test:
sources: 	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
$(THIS).o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
