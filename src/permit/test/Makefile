# $Header: /users/source/archives/cm_tools.vcs/src/permit/test/RCS/Makefile,v 1.2 1989/03/28 14:01:15 dickey Exp $
# make-file for testing RCS permission utility
#
# $Log: Makefile,v $
# Revision 1.2  1989/03/28 14:01:15  dickey
# write test results (including stderr) to both terminal and run_tests.out
#
#	Revision 1.1  89/03/23  14:01:23  dickey
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
