# $Header: /users/source/archives/cm_tools.vcs/src/checkout/RCS/Makefile,v 3.0 1989/03/28 10:15:23 ste_cm Rel $
# Top-level makefile for RCS checkout utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 10:15:23  ste_cm
# BASELINE Mon Jun 19 13:11:22 EDT 1989
#
#	Revision 2.0  89/03/28  10:15:23  ste_cm
#	BASELINE Thu Apr  6 09:18:19 EDT 1989
#	
#	Revision 1.4  89/03/28  10:15:23  dickey
#	use MAKE-variable to encapsulate recursive-build info.
#	
#	Revision 1.3  89/03/27  14:02:48  dickey
#	integration/cleanup for recursive make
#	
#	Revision 1.2  89/03/27  10:46:39  dickey
#	maintain 'rcsget' script in same directory as 'checkout' module
#	
#	Revision 1.1  89/03/24  14:13:33  dickey
#	Initial revision
#	

####### (Development) ##########################################################
B	= ../../bin
L	= ../common/lib
PUT	= copy -v -d ../$@
GET	= checkout
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)

####### (Standard Lists) #######################################################
PROG	= checkout
ALL	=\
	$B/rcsget\
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
run_tests\
sources\
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
$B/rcsget:	bin/rcsget	; cd bin;	$(PUT)
