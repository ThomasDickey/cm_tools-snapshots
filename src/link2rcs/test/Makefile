# $Header: /users/source/archives/cm_tools.vcs/src/link2rcs/test/RCS/Makefile,v 3.0 1989/04/03 08:30:25 ste_cm Rel $
# make-file for testing RCS skeleton tree utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/04/03 08:30:25  ste_cm
# BASELINE Mon Jun 19 14:46:26 EDT 1989
#
#	Revision 2.0  89/04/03  08:30:25  ste_cm
#	BASELINE Thu Apr  6 13:50:00 EDT 1989
#	
#	Revision 1.3  89/04/03  08:30:25  dickey
#	added lint & lincnt rules
#	
#	Revision 1.2  89/03/28  13:18:18  dickey
#	write test result to run_tests.out
#	
#	Revision 1.1  89/03/27  08:48:25  dickey
#	Initial revision
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
lint.out:
lincnt.out:
run_tests:	$(SOURCES)	; sh -c '$@.sh 2>&1 | tee -a $@.out'

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
