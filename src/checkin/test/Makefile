# $Header: /users/source/archives/cm_tools.vcs/src/checkin/test/RCS/Makefile,v 1.2 1989/03/28 11:59:11 dickey Exp $
# make-file for testing RCS checkin utility
#
# $Log: Makefile,v $
# Revision 1.2  1989/03/28 11:59:11  dickey
# pipe the test result to run_tests.out
#
#	Revision 1.1  89/03/27  09:51:56  dickey
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
run_tests:	$(SOURCES)	; sh -c '$@.sh 2>&1 | tee -a $@.out'

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
