# $Id: Makefile,v 9.0 1989/08/22 09:40:32 ste_cm Rel $
# make-file for testing RCS/sccs checkup utility
#
# $Log: Makefile,v $
# Revision 9.0  1989/08/22 09:40:32  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
#	Revision 8.0  89/08/22  09:40:32  ste_cm
#	BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
#	
#	Revision 7.0  89/08/22  09:40:32  ste_cm
#	BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
#	
#	Revision 6.0  89/08/22  09:40:32  ste_cm
#	BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
#	
#	Revision 5.0  89/08/22  09:40:32  ste_cm
#	BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#	
#	Revision 4.0  89/08/22  09:40:32  ste_cm
#	BASELINE Thu Aug 24 09:32:43 EDT 1989 -- support:navi_011(rel2)
#	
#	Revision 3.1  89/08/22  09:40:32  dickey
#	corrected 'destroy' rule
#	
#	Revision 3.0  89/04/03  08:22:45  ste_cm
#	BASELINE Mon Jun 19 13:19:10 EDT 1989
#	

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sh

SOURCES	= Makefile $(SCRIPTS)

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
