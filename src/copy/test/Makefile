# $Header: /users/source/archives/cm_tools.vcs/src/copy/test/RCS/Makefile,v 1.1 1989/03/24 13:26:39 dickey Exp $
# make-file for testing unix copy utility
#
# $Log: Makefile,v $
# Revision 1.1  1989/03/24 13:26:39  dickey
# Initial revision
#

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sh

SOURCES	= $(SCRIPTS)

####### (Standard Productions) #################################################
all:
clean:				; rm -f *.out
clobber:	clean
destroy:			; rm -f *
sources:	$(SOURCES)
run_tests:	$(SOURCES)	; $@.sh

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
