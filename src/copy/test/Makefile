# $Header: /users/source/archives/cm_tools.vcs/src/copy/test/RCS/Makefile,v 3.0 1989/04/03 08:23:43 ste_cm Rel $
# make-file for testing unix copy utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/04/03 08:23:43  ste_cm
# BASELINE Mon Jun 19 14:19:05 EDT 1989
#
#	Revision 2.0  89/04/03  08:23:43  ste_cm
#	BASELINE Thu Apr  6 13:10:28 EDT 1989
#	
#	Revision 1.2  89/04/03  08:23:43  dickey
#	added rules for lint & lincnt
#	
#	Revision 1.1  89/03/24  13:26:39  dickey
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
run_tests:	$(SOURCES)	; $@.sh

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
