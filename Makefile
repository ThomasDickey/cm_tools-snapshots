# $Id: Makefile,v 11.7 1994/11/09 00:15:59 tom Exp $
# Top-level make-file for CM_TOOLS

THIS	= cm_tools
TOP	= ..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
SOURCES	=\
	COPYING\
	README

MFILES	=\
	bin/Makefile\
	certify/Makefile\
	src/Makefile\
	user/Makefile

####### (Standard Productions) #################################################
all\
sources::	$(SOURCES)

all\
clean\
clobber\
destroy\
run_test\
install\
deinstall\
sources::	$(MFILES)
	cd certify;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@ INSTALL_MAN=`cd ..;cd $(INSTALL_MAN);pwd`
	cd bin;		$(MAKE) $@ INSTALL_BIN=`cd ..;cd $(INSTALL_BIN);pwd`

all::
	cd src;		$(MAKE) install

lint.out::
	cd src;		$(MAKE) $@
clean\
clobber::			; -$(RM) $(CLEAN)
destroy::			; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES)\
$(SOURCES):			; $(GET) $@
