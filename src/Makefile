# $Id: Makefile,v 10.0 1991/10/11 08:50:11 ste_cm Rel $
# Top-level makefile for CM_TOOLS src-directory
#

####### (Development) ##########################################################
COPY	= cp -p
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	CFLAGS="$(CFLAGS)" COPY="$(COPY)"

####### (Standard Lists) #######################################################
MFILES	=\
	common/Makefile\
	baseline/Makefile\
	checkin/Makefile\
	checkout/Makefile\
	checkup/Makefile\
	copy/Makefile\
	ded/Makefile\
	link2rcs/Makefile\
	permit/Makefile\
	vcs/Makefile

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
deinstall::	$(MFILES)
	cd common;	$(MAKE) $@
	cd baseline;	$(MAKE) $@
	cd checkin;	$(MAKE) $@
	cd checkout;	$(MAKE) $@
	cd checkup;	$(MAKE) $@
	cd copy;	$(MAKE) $@
	cd ded;		$(MAKE) $@
	cd link2rcs;	$(MAKE) $@
	cd permit;	$(MAKE) $@
	cd vcs;		$(MAKE) $@

destroy::
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

rdestroy: destroy

####### (Details of Productions) ###############################################
$(MFILES):		; checkout -x $@
