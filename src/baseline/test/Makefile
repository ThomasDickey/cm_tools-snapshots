# $Header: /users/source/archives/cm_tools.vcs/src/baseline/test/RCS/Makefile,v 1.2 1989/03/28 11:48:42 dickey Exp $
# make-file for testing RCS skeleton tree utility
#
# $Log: Makefile,v $
# Revision 1.2  1989/03/28 11:48:42  dickey
# pipe the test-result to run_tests.out
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
run_tests:	$(SOURCES)	; sh -c '$@.sh 2>&1 | tee -a $@.out'

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
