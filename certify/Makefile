# $Id: Makefile,v 11.2 1993/04/27 10:40:51 dickey Exp $

####### (Development) ##########################################################
TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SOURCES	=\
	README\
	config.txt\
	release.txt\
	reusable.txt

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; rm -f $(CLEAN)
clobber:	clean
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)
install:
deinstall:

####### (Details of Productions) ###############################################
$(SOURCES):	; $(GET) $@
