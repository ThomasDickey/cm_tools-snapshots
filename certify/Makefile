# $Id: Makefile,v 10.0 1989/08/22 08:35:16 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 10.0  1989/08/22 08:35:16  ste_cm
# BASELINE Fri Oct 18 16:49:43 1991 -- ANSI conversion; vcs
#
#	Revision 9.0  89/08/22  08:35:16  ste_cm
#	BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#	
#	Revision 8.0  89/08/22  08:35:16  ste_cm
#	BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
#	
#	Revision 7.0  89/08/22  08:35:16  ste_cm
#	BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
#	
#	Revision 6.0  89/08/22  08:35:16  ste_cm
#	BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
#	
#	Revision 5.0  89/08/22  08:35:16  ste_cm
#	BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#	
#	Revision 4.0  89/08/22  08:35:16  ste_cm
#	BASELINE Thu Aug 24 09:15:10 EDT 1989 -- support:navi_011(rel2)
#	
#	Revision 3.1  89/08/22  08:35:16  dickey
#	corrected 'destroy' rule; added 'reusable_parts.txt'
#	
#	Revision 3.0  89/03/27  16:22:13  ste_cm
#	BASELINE Mon Jun 19 12:57:11 EDT 1989
#	

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SOURCES	=\
	README\
	config_sheet.txt\
	release_notes.txt\
	reusable_parts.txt

####### (Standard Productions) #################################################
all:		sources
clean:
clobber:
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)
install:
deinstall:

####### (Details of Productions) ###############################################
$(SOURCES):	; $(GET) $@
