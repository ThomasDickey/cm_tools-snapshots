# $Header: /users/source/archives/cm_tools.vcs/src/checkup/test/RCS/Makefile,v 1.2 1989/03/28 11:25:01 dickey Exp $
# make-file for testing RCS/sccs checkup utility
#
# $Log: Makefile,v $
# Revision 1.2  1989/03/28 11:25:01  dickey
# pipe test-result to run_tests.out as well as to terminal
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
run_tests:	$(SOURCES)	; sh -c '$@.sh 2>&1 | tee -a $@.out'

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
