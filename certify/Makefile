# $Id: Makefile,v 11.1 1992/10/09 12:39:04 dickey Exp $

####### (Development) ##########################################################
TOP	= ../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
SOURCES	=\
	README\
	config_sheet.txt\
	release_notes.txt\
	reusable_parts.txt

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; rm -f $(CLEAN)
clobber:	clean
destroy:			; $(DESTROY)
run_tests:
sources:	$(SOURCES)
install:
deinstall:

####### (Details of Productions) ###############################################
$(SOURCES):	; $(GET) $@
