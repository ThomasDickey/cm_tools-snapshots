# $Id: Makefile,v 11.2 1992/10/09 13:01:04 dickey Exp $
# Top-level makefile for RCS baseline utility

####### (Development) ##########################################################
top	= ../..
TOP	= $(top)/..
include $(TOP)/cm_library/support/cm_library.mk

B	= $(top)/bin

####### (Standard Lists) #######################################################
THIS	= baseline
ALL	=\
	$B/$(THIS)
MFILES	=\
	src/Makefile\
	test/Makefile

####### (Standard Productions) #################################################
all\
run_tests::	$L/$(CM_LIB).a bin

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

clean\
clobber::			; rm -f $(CLEAN)
clobber\
destroy::			; rm -rf bin
destroy::			; $(DESTROY)

install:	all $(ALL)
deinstall:			; rm -f $(ALL)

####### (Details of Productions) ###############################################
bin:				; mkdir $@
$(MFILES):			; $(GET) $@
$B/$(THIS):	bin/$(THIS)	; $(PUT)
