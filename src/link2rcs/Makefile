# $Id: Makefile,v 9.0 1991/06/05 14:37:10 ste_cm Rel $
# Top-level makefile for RCS skeleton tree utility
#
# $Log: Makefile,v $
# Revision 9.0  1991/06/05 14:37:10  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
#	Revision 8.1  91/06/05  14:37:10  dickey
#	cleanup install-rule
#	
#	Revision 8.0  89/08/22  09:07:11  ste_cm
#	BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
#	

####### (Development) ##########################################################
B	= ../../bin
L	= ../common/lib
GET	= checkout
PUT	= ../$B/copy -d ../$@
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	GET=$(GET)

####### (Standard Lists) #######################################################
PROG	= link2rcs
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
run_tests\
sources\
lincnt.out\
lint.out:	$(FIRST)
	cd src;		$(MAKE) $@
	cd test;	$(MAKE) $@

rdestroy\
destroy:	$(FIRST)
	cd src;		$(MAKE) destroy
	cd test;	$(MAKE) destroy
	rm -rf bin
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

install:	all $(ALL)
deinstall:		; rm -f $(ALL)

####### (Details of Productions) ###############################################
bin:				; mkdir $@
src/Makefile:			; cd src;	$(GET) Makefile
test/Makefile:			; cd test;	$(GET) Makefile
$B/$(PROG):	bin/$(PROG)	; cd bin;	$(PUT)
