# $Id: Makefile,v 11.3 1992/10/16 09:24:57 dickey Exp $
# MAKE-file for RCS skeleton tree utility

####### (Environment) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

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
run_tests:
sources: 	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
$(THIS).o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
