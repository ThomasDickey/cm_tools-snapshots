# $Header: /users/source/archives/cm_tools.vcs/RCS/Makefile,v 3.0 1989/03/29 07:35:54 ste_cm Rel $
# Top-level make-file for CM_TOOLS
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/29 07:35:54  ste_cm
# BASELINE Mon Jun 19 12:54:05 EDT 1989
#
#	Revision 2.0  89/03/29  07:35:54  ste_cm
#	BASELINE Tue Apr  4 15:54:07 EDT 1989
#	
#	Revision 1.7  89/03/29  07:35:54  dickey
#	added missing Makefile-dependencies
#	
#	Revision 1.6  89/03/28  16:36:33  dickey
#	added 'make' rules for user-documentation directory
#	
#	Revision 1.5  89/03/28  16:13:41  dickey
#	source-arg of 'copy.sh' must be a simple file...
#	
#	Revision 1.4  89/03/28  14:39:17  dickey
#	corrected INSTALL_PATH default value
#	
#	Revision 1.3  89/03/28  10:47:02  dickey
#	use MAKE-variable to encapsulate recursive-make info.
#	
#	Revision 1.2  89/03/27  16:26:41  dickey
#	cleanup of top-level, prepare for build on sun/gould
#	
#	Revision 1.1  89/03/27  14:24:40  dickey
#	RCS_BASE
#	
#	Revision 7.0  89/10/05  09:45:39  ste_cm
#	BASELINE Mon Apr 30 12:49:21 1990 -- (CPROTO)
#	
####### (Development) ##########################################################
INSTALL_PATH = /ste_site/ste/bin
INSTALL_DOCS = /ste_site/ste/doc
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)
THIS	= Makefile
CFLAGS	=
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS) GET=$(GET) CFLAGS="$(CFLAGS)"

####### (Standard Lists) #######################################################
SOURCES	=\
	COPYRIGHT\
	README

	bin/makefile\
MFILES	=\
	support/Makefile\
	bin/Makefile\
MKFILE	=\
	bin/makefile\
	user/makefile

DIRS	=\
	interface\
	bin/copy
HACKS	=\
	bin/copy\
	interface/rcspath.h

	$(SOURCES)\
	$(HACKS)\
	interface/rcspath.h
	$(MKFILE)\
	$(DIRS)\
	$(HACKS)

####### (Standard Productions) #################################################
all:		$(FIRST)
	cd certificate;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) install
clean\
clobber\
lincnt.out:	$(FIRST)
	cd src;		$(MAKE) $@

run_tests\
sources\
install\
deinstall:	$(FIRST)
	cd certificate;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) $@
destroy:	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@
	rm -f lib/* interface/*
	rmdir $(DIRS)
	rm -f *
destroy::
	rm -rf $(DIRS)
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

####### (Details of Productions) ###############################################
.first:		$(FIRST)

bin/Makefile:			; cd bin;		$(GET) Makefile
certificate/Makefile:		; cd certificate;	$(GET) Makefile
support/Makefile:		; cd support;		$(GET) Makefile
src/Makefile:			; cd src;		$(GET) Makefile
user/Makefile:			; cd user;		$(GET) Makefile
support/Makefile\
src/Makefile\
user/Makefile:			; $(GET) -x $@

# Embed default installation path in places where we want it compiled-in.
# Note that we exploit the use of lower-case makefile for this purpose.
user/makefile:	user/Makefile	$(THIS)
	rm -f $@
	sed -e s+INSTALL_PATH=.*+INSTALL_PATH=$(INSTALL_DOCS)+ user/Makefile >$@

# If the rcs tool is not found in our path, assume that it lies in the same
# directory as that in which we will install; but the install-directory has
# been removed from our path as part of a clean-build.
interface/rcspath.h:		$(THIS)
	rm -f $@
	echo "#define	RCS_PATH	\"$(RCS_PATH)\"" >$@
	sh -c 'if ( grep "\"no\ rcs\ in" $@ )\
		then echo "#define RCS_PATH \"$(INSTALL_PATH)/\"" >$@;\
		else echo found rcs-path; fi'

# We use the 'copy' utility rather than the unix 'cp' utility, since it
# preserves file-dates.  This is normally not in your path when first building
# it!  The following rules provide a temporary version of 'copy' which lives
# in the bin-directory; you should put this directory in your path to take
# advantage of it:
bin/copy:
	cd support; $(MAKE) copy.sh; ./copy.sh copy.sh ../$@
