# $Id: Makefile,v 11.3 1992/10/16 10:00:32 dickey Exp $
# Top-level make-file for CM_TOOLS

THIS	= cm_tools
TOP	= ..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
SOURCES	=\
	COPYRIGHT\
	README

MFILES	=\
	bin/Makefile\
	certificate/Makefile\
	src/Makefile\
	user/Makefile

####### (Standard Productions) #################################################
all\
sources::	$(SOURCES)

all\
clean\
clobber\
destroy\
run_tests\
install\
deinstall\
sources::	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@ INSTALL_MAN=`cd ..;cd $(INSTALL_MAN);pwd`
	cd bin;		$(MAKE) $@ INSTALL_BIN=`cd ..;cd $(INSTALL_BIN);pwd`

all::
	cd src;		$(MAKE) install

lint.out\
lincnt.out::
	cd src;		$(MAKE) $@
clean\
clobber::			; rm -f $(CLEAN)
destroy::			; $(DESTROY)

####### (Details of Productions) ###############################################
$(MFILES)\
$(SOURCES):			; $(GET) $@
