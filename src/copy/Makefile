# $Id: Makefile,v 11.0 1992/02/27 09:56:47 ste_cm Rel $
# Top-level makefile for unix-copy utility

####### (Development) ##########################################################
B	= ../../bin
L	= ../common/lib

GET	= checkout
COPY	= cp -p
PUT	= rm -f $@; $(COPY) $? $@
MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	CFLAGS="$(CFLAGS)" COPY="$(COPY)"

####### (Standard Lists) #######################################################
PROG	= copy
ALL	=\
	$B/$(PROG)
MFILES	=\
	src/Makefile\
	test/Makefile

####### (Standard Productions) #################################################
all::		bin $L/lib.a

all\
clean\
clobber\
destroy\
run_tests\
sources\
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
$(MFILES):			; $(GET) $@
$B/$(PROG):	bin/$(PROG)	; $(PUT)
