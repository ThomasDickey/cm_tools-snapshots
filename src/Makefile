# $Id: Makefile,v 9.0 1990/05/08 13:57:16 ste_cm Rel $
# Top-level makefile for CM_TOOLS src-directory
#
# $Log: Makefile,v $
# Revision 9.0  1990/05/08 13:57:16  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
#	Revision 8.0  90/05/08  13:57:16  ste_cm
#	BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
#	
#	Revision 7.1  90/05/08  13:57:16  dickey
#	simplified using "::" rules and "-x" option of checkout
#	
#	Revision 7.0  89/09/28  11:27:01  ste_cm
#	BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
#	

####### (Development) ##########################################################
GET	= checkout
CFLAGS	=
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET) CFLAGS="$(CFLAGS)"

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
destroy\
sources\
lint.out\
lincnt.out\
run_tests\
install\
deinstall::	$(FIRST)
	cd common;	$(MAKE) $@
	cd baseline;	$(MAKE) $@
	cd checkin;	$(MAKE) $@
	cd checkout;	$(MAKE) $@
	cd checkup;	$(MAKE) $@
	cd copy;	$(MAKE) $@
	cd ded;		$(MAKE) $@
	cd link2rcs;	$(MAKE) $@
	cd permit;	$(MAKE) $@

destroy::
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

rdestroy: destroy

####### (Details of Productions) ###############################################
baseline/Makefile\
checkin/Makefile\
checkout/Makefile\
checkup/Makefile\
common/Makefile\
copy/Makefile\
ded/Makefile\
link2rcs/Makefile\
permit/Makefile:	; $(GET) -x $@
