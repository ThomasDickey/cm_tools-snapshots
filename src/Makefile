# $Header: /users/source/archives/cm_tools.vcs/src/RCS/Makefile,v 3.0 1989/03/28 10:16:06 ste_cm Rel $
# Top-level makefile for CM_TOOLS src-directory
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 10:16:06  ste_cm
# BASELINE Mon Jun 19 13:00:20 EDT 1989
#
#	Revision 2.0  89/03/28  10:16:06  ste_cm
#	BASELINE Tue Apr  4 16:04:14 EDT 1989
#	
#	Revision 1.3  89/03/28  10:16:06  dickey
#	use MAKE-variable to encapsulate recursive-build info.
#	
#	Revision 1.2  89/03/27  13:19:27  dickey
#	integration/cleanup for recursive make
#	
#	Revision 1.1  89/03/27  11:51:51  dickey
#	RCS_BASE
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

destroy: rdestroy	; rm -f *

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
