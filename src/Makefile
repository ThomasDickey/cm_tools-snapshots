# $Id: Makefile,v 11.5 1993/05/06 13:47:17 dickey Exp $
# Top-level makefile for CM_TOOLS src-directory

####### (Development) ##########################################################
TOP	= ../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
MFILES	=\
	baseline/Makefile\
	checkin/Makefile\
	checkout/Makefile\
	checkup/Makefile\
	copy/Makefile\
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
run_test\
install\
deinstall::	$(MFILES)
	cd baseline;	$(MAKE) $@
	cd checkin;	$(MAKE) $@
	cd checkout;	$(MAKE) $@
	cd checkup;	$(MAKE) $@
	cd copy;	$(MAKE) $@
	cd link2rcs;	$(MAKE) $@
	cd permit;	$(MAKE) $@
	cd vcs;		$(MAKE) $@

clean\
clobber::		; rm -f $(CLEAN)

destroy::		; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES):		; $(GET) $@
