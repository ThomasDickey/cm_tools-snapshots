# $Id: Makefile,v 11.3 1992/10/16 09:32:37 dickey Exp $
# Top-level makefile for CM_TOOLS src-directory

####### (Development) ##########################################################
TOP	= ../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
MFILES	=\
	baseline/Makefile\
	checkin/Makefile\
	checkout/Makefile\
	checkup/Makefile\
	copy/Makefile\
	ded/Makefile\
	link2rcs/Makefile\
	permit/Makefile\
	vcs/Makefile

SOURCES	=\
	Makefile

####### (Standard Productions) #################################################
all::		$(SOURCES)

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
clobber::		; rm -f $(CLEAN)

destroy::		; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES):		; $(GET) $@
