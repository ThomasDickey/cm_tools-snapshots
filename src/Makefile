# $Id: Makefile,v 7.0 1989/09/28 11:27:01 ste_cm Rel $
# Top-level makefile for CM_TOOLS src-directory
#
# $Log: Makefile,v $
# Revision 7.0  1989/09/28 11:27:01  ste_cm
# BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
#
#	Revision 6.0  89/09/28  11:27:01  ste_cm
#	BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
#	
#	Revision 5.0  89/09/28  11:27:01  ste_cm
#	BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#	
#	Revision 4.1  89/09/28  11:27:01  dickey
#	deleted 'rdiff' utility from this tree
#	
#	Revision 4.0  89/08/22  08:43:05  ste_cm
#	BASELINE Thu Aug 24 09:17:54 EDT 1989 -- support:navi_011(rel2)
#	
#	Revision 3.1  89/08/22  08:43:05  dickey
#	corrected 'destroy' rule
#	
#	Revision 3.0  89/03/28  10:16:06  ste_cm
#	BASELINE Mon Jun 19 13:00:20 EDT 1989
#	

####### (Development) ##########################################################
GET	= checkout
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)

####### (Standard Lists) #######################################################
FIRST	=\
	common/Makefile\
	baseline/Makefile\
	checkin/Makefile\
	checkout/Makefile\
	checkup/Makefile\
	copy/Makefile\
	ded/Makefile\
	link2rcs/Makefile\
	permit/Makefile

####### (Standard Productions) #################################################
all\
clean\
clobber\
rdestroy\
sources\
lint.out\
lincnt.out\
run_tests\
install\
deinstall:	$(FIRST)
	cd common;	$(MAKE) $@
	cd baseline;	$(MAKE) $@
	cd checkin;	$(MAKE) $@
	cd checkout;	$(MAKE) $@
	cd checkup;	$(MAKE) $@
	cd copy;	$(MAKE) $@
	cd ded;		$(MAKE) $@
	cd link2rcs;	$(MAKE) $@
	cd permit;	$(MAKE) $@

destroy: rdestroy
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

####### (Details of Productions) ###############################################
baseline/Makefile:	; cd baseline;	$(GET) Makefile
checkin/Makefile:	; cd checkin;	$(GET) Makefile
checkout/Makefile:	; cd checkout;	$(GET) Makefile
checkup/Makefile:	; cd checkup;	$(GET) Makefile
common/Makefile:	; cd common;	$(GET) Makefile
copy/Makefile:		; cd copy;	$(GET) Makefile
ded/Makefile:		; cd ded;	$(GET) Makefile
link2rcs/Makefile:	; cd link2rcs;	$(GET) Makefile
permit/Makefile:	; cd permit;	$(GET) Makefile
