# $Header: /users/source/archives/cm_tools.vcs/RCS/Makefile,v 4.0 1989/08/22 08:24:46 ste_cm Rel $
# Top-level make-file for CM_TOOLS
#
# $Log: Makefile,v $
# Revision 4.0  1989/08/22 08:24:46  ste_cm
# BASELINE Thu Aug 24 09:12:03 EDT 1989 -- support:navi_011(rel2)
#
#	Revision 3.2  89/08/22  08:24:46  dickey
#	'destroy' rule should be error-free
#	
#	Revision 3.1  89/08/22  08:16:43  dickey
#	corrected/revised 'clean', 'clobber' & 'destroy' rules
#	
#	Revision 3.0  89/03/29  07:35:54  ste_cm
#	BASELINE Mon Jun 19 12:54:05 EDT 1989
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
clean:		$(MFILES)
clean\
clobber\
destroy::	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) $@
clobber:	$(MFILES)
	cd certificate;	$(MAKE) $@
	cd support;	$(MAKE) $@
	cd src;		$(MAKE) $@
	cd user;	$(MAKE) $@
	cd bin;		$(MAKE) $@
	rm -f $(HACKS) $(MKFILE)
lint.out\
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
	cd bin;		$(MAKE) $@ INSTALL_PATH=$(INSTALL_PATH)

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
bin/makefile:	bin/Makefile	$(THIS)
	rm -f $@
	sed -e s+INSTALL_PATH=.*+INSTALL_PATH=$(INSTALL_PATH)+ bin/Makefile >$@

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
