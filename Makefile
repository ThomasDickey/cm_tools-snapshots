# $Header: /users/source/archives/cm_tools.vcs/RCS/Makefile,v 9.1 1991/06/18 14:03:37 dickey Exp $
# Top-level make-file for CM_TOOLS
# (see also CM_TOOLS/src/common)
#
# $Log: Makefile,v $
# Revision 9.1  1991/06/18 14:03:37  dickey
# correction to install-doc
#
#	Revision 9.0  91/06/05  14:21:52  ste_cm
#	BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#	
####### (Development) ##########################################################
INSTALL_BIN = ~ste_cm/bin/`arch`
INSTALL_DOC = /ste_site/ste/doc
COPY	= cp -p
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS) CFLAGS="$(CFLAGS)"
THIS	= cm_tools

RCS_PATH= `which rcs | sed -e s+/rcs$$+/+`

####### (Standard Lists) #######################################################
SOURCES	=\
	COPYRIGHT\
	README

MFILES	=\
	support/Makefile\
	bin/Makefile\
	certificate/Makefile\
	src/Makefile\
	user/Makefile
MKFILE	=\
	bin/makefile\
	user/makefile

DIRS	=\
	interface\
	lib

HACKS	=\
	bin/copy\
	interface/rcspath.h

FIRST	=\
	$(SOURCES)\
	$(MFILES)\
	$(MKFILE)\
	$(DIRS)\
	$(HACKS)

####### (Standard Productions) #################################################
all:		$(FIRST)
	cd certificate;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) install
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@

clean\
clobber\
destroy::	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@

clobber::
	rm -rf $(HACKS) $(MKFILE) $(DIRS)

lint.out\
lincnt.out:	$(FIRST)
	cd src;		$(MAKE) $@

run_tests\
sources\
install:	$(FIRST)
	cd certificate;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@ INSTALL_PATH=$(INSTALL_DOC)
	cd bin;		$(MAKE) $@ INSTALL_PATH=$(INSTALL_BIN)

deinstall:	bin/makefile
	cd bin;		$(MAKE) $@ INSTALL_PATH=$(INSTALL_BIN)

destroy::
	rm -rf $(DIRS)
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

####### (Details of Productions) ###############################################
.first:		$(FIRST)

$(MFILES)\
$(SOURCES):			; checkout -x $@
$(DIRS):			; mkdir $@

# Embed default installation path in places where we want it compiled-in.
# Note that we exploit the use of lower-case makefile for this purpose.
bin/makefile:	bin/Makefile	Makefile
	rm -f $@
	sed -e s+INSTALL_PATH=.*+INSTALL_PATH=$(INSTALL_BIN)+ bin/Makefile >$@

user/makefile:	user/Makefile	Makefile
	rm -f $@
	sed -e s+INSTALL_PATH=.*+INSTALL_PATH=$(INSTALL_DOC)+ user/Makefile >$@

# If the rcs tool is not found in our path, assume that it lies in the same
# directory as that in which we will install; but the install-directory has
# been removed from our path as part of a clean-build.
interface/rcspath.h:		Makefile
	rm -f $@
	echo "#define	RCS_PATH	\"$(RCS_PATH)\"" >$@
	sh -c 'if ( grep "\"no\ rcs\ in" $@ )\
		then echo "#define RCS_PATH \"$(INSTALL_BIN)/\"" >$@;\
		else echo found rcs-path; fi'

# We use the 'copy' utility rather than the unix 'cp' utility, since it
# preserves file-dates.  This is normally not in your path when first building
# it!  The following rules provide a temporary version of 'copy' which lives
# in the bin-directory; you should put this directory in your path to take
# advantage of it:
bin/copy:
	cd support; $(MAKE) copy.sh; ./copy.sh copy.sh ../$@
