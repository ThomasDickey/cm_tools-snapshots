# $Id: Makefile,v 4.0 1989/08/22 08:43:05 ste_cm Rel $
# Top-level makefile for CM_TOOLS src-directory
#
# $Log: Makefile,v $
# Revision 4.0  1989/08/22 08:43:05  ste_cm
# BASELINE Thu Aug 24 09:17:54 EDT 1989 -- support:navi_011(rel2)
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
	permit/Makefile\
	rdiff/Makefile

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
	cd rdiff;	$(MAKE) $@

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
rdiff/Makefile:		; cd rdiff;	$(GET) Makefile
