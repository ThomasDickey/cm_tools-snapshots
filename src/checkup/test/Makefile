# $Header: /users/source/archives/cm_tools.vcs/src/checkup/test/RCS/Makefile,v 3.0 1989/04/03 08:22:45 ste_cm Rel $
# make-file for testing RCS/sccs checkup utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/04/03 08:22:45  ste_cm
# BASELINE Mon Jun 19 13:19:10 EDT 1989
#
#	Revision 2.0  89/04/03  08:22:45  ste_cm
#	BASELINE Thu Apr  6 09:34:48 EDT 1989
#	
#	Revision 1.3  89/04/03  08:22:45  dickey
#	added rules for lint & lincnt
#	
#	Revision 1.2  89/03/28  11:25:01  dickey
#	pipe test-result to run_tests.out as well as to terminal
#	
#	Revision 1.1  89/03/24  13:48:05  dickey
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
