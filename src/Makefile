# $Id: Makefile,v 11.6 1994/11/09 00:15:59 tom Exp $
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
clobber::		; -$(RM) $(CLEAN)

destroy::		; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES):		; $(GET) $@
