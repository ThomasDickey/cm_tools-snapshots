# $Id: Makefile,v 11.4 1994/11/09 00:15:59 tom Exp $

####### (Development) ##########################################################
TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SOURCES	=\
	README\
	config.txt\
	release.txt

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; -$(RM) $(CLEAN)
clobber:	clean
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)
install:
deinstall:

####### (Details of Productions) ###############################################
$(SOURCES):	; $(GET) $@
