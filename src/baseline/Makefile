# $Id: Makefile,v 10.0 1991/10/18 12:17:27 ste_cm Rel $
# Top-level makefile for RCS baseline utility

####### (Development) ##########################################################
B	= ../../bin
L	= ../common/lib

COPY	= cp -p
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	CFLAGS="$(CFLAGS)" COPY="$(COPY)"

####### (Standard Lists) #######################################################
PROG	= baseline
ALL	=\
	$B/$(PROG)
MFILES	=\
	src/Makefile\
	test/Makefile

####### (Standard Productions) #################################################
all\
run_tests::	bin $L/lib.a

all\
clean\
clobber\
destroy\
sources\
run_tests\
lincnt.out\
lint.out::	$(MFILES)
	cd src;		$(MAKE) $@
	cd test;	$(MAKE) $@

clobber\
destroy::
	rm -rf bin
destroy::
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

install:	all $(ALL)
deinstall:		; rm -f $(ALL)

####### (Details of Productions) ###############################################
bin:				; mkdir $@
$(MFILES):			; checkout -x $@
$B/$(PROG):	bin/$(PROG)	; $(COPY) $? $@
