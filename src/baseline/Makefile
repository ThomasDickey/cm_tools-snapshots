# $Header: /users/source/archives/cm_tools.vcs/src/baseline/RCS/Makefile,v 3.0 1989/03/28 10:19:51 ste_cm Rel $
# Top-level makefile for RCS baseline utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 10:19:51  ste_cm
# BASELINE Mon Jun 19 13:01:15 EDT 1989
#
#	Revision 2.0  89/03/28  10:19:51  ste_cm
#	BASELINE Tue Apr  4 16:05:09 EDT 1989
#	
#	Revision 1.3  89/03/28  10:19:51  dickey
#	use MAKE-variable to encapsulate recursive-build info.
#	
#	Revision 1.2  89/03/27  14:02:35  dickey
#	integration/cleanup for recursive make
#	
#	Revision 1.1  89/03/27  09:04:57  dickey
#	Initial revision
#	

####### (Development) ##########################################################
B	= ../../bin
L	= ../common/lib
PUT	= copy -v -d ../$@
GET	= checkout
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)

####### (Standard Lists) #######################################################
PROG	= baseline
ALL	=\
	$B/$(PROG)
FIRST	=\
	bin\
	src/Makefile\
	test/Makefile

####### (Standard Productions) #################################################
all:		$L/lib.a

all\
clean\
clobber\
sources\
run_tests\
lincnt.out\
lint.out:	$(FIRST)
	cd src;		$(MAKE) $@
	cd test;	$(MAKE) $@

rdestroy\
destroy:	$(FIRST) clobber
	cd src;		$(MAKE) destroy
	cd test;	$(MAKE) destroy
	rmdir bin
	rm -f *

install:	all $(ALL)
deinstall:		; rm -f $(ALL)

####### (Details of Productions) ###############################################
bin:				; mkdir $@
src/Makefile:			; cd src;	$(GET) Makefile
test/Makefile:			; cd test;	$(GET) Makefile
$B/$(PROG):	bin/$(PROG)	; cd bin;	$(PUT)
