# $Id: Makefile,v 11.1 1992/08/05 13:52:47 dickey Exp $
# Top-level makefile for CM_TOOLS src-directory

####### (Development) ##########################################################
GET	= checkout
COPY	= cp -p
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	CFLAGS="$(CFLAGS)" COPY="$(COPY)"

####### (Standard Lists) #######################################################
MFILES	=\
	common.mk\
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

clean\
clobber::
	rm -f *.bak *.log *.out core

destroy::
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

lintlib::	$(MFILES)
	cd common;	$(MAKE) $@

####### (Details of Productions) ###############################################
$(MFILES):		; $(GET) $@
