# $Id: Makefile,v 4.1 1989/09/06 13:09:27 dickey Exp $
# make-file for testing RCS permission utility
#
# $Log: Makefile,v $
# Revision 4.1  1989/09/06 13:09:27  dickey
# RCS_BASE
#
#	Revision 4.0  89/08/22  09:01:33  ste_cm
#	BASELINE Thu Aug 24 10:41:43 EDT 1989 -- support:navi_011(rel2)
#	
#	Revision 3.1  89/08/22  09:01:33  dickey
#	corrected 'destroy' rule
#	
#	Revision 3.0  89/04/03  08:32:04  ste_cm
#	BASELINE Mon Jun 19 14:50:57 EDT 1989
#	

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sh

SOURCES	= $(SCRIPTS)

####### (Standard Productions) #################################################
all:		sources
clean:				; rm -f *.out
clobber:	clean
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_tests:	$(SOURCES)	; sh -c '$@.sh 2>&1 | tee -a $@.out'

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
