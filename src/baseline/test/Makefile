# $Header: /users/source/archives/cm_tools.vcs/src/baseline/test/RCS/Makefile,v 3.0 1989/04/03 08:18:07 ste_cm Rel $
# make-file for testing RCS skeleton tree utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/04/03 08:18:07  ste_cm
# BASELINE Mon Jun 19 13:04:14 EDT 1989
#
#	Revision 2.0  89/04/03  08:18:07  ste_cm
#	BASELINE Tue Apr  4 16:07:27 EDT 1989
#	
#	Revision 1.3  89/04/03  08:18:07  dickey
#	added rules for lint & lincnt
#	
#	Revision 1.2  89/03/28  11:48:42  dickey
#	pipe the test-result to run_tests.out
#	
#	Revision 1.1  89/03/27  09:14:47  dickey
#	Initial revision
#	

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sed\
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
