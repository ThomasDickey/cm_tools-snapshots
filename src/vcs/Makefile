# $Id: Makefile,v 11.4 1994/11/09 00:17:28 tom Exp $
# Top-level makefile for RCS admin utility

####### (Development) ##########################################################
top	= ../..
TOP	= $(top)/..
include $(TOP)/td_lib/support/td_lib.mk

B	= $(top)/bin

####### (Standard Lists) #######################################################
THIS	= vcs
ALL	=\
	$B/$(THIS)
MFILES	=\
	src/Makefile\
	test/Makefile

####### (Standard Productions) #################################################
all\
run_tests::	$L/$(TD_LIB).a bin

all\
clean\
clobber\
destroy\
sources\
run_tests\
lint.out::	$(MFILES)
	cd src;		$(MAKE) $@
	cd test;	$(MAKE) $@

clean\
clobber::			; -$(RM) $(CLEAN)
clobber\
destroy::			; -$(RM) -r bin
destroy::			; $(DESTROY)

install:	all $(ALL)
deinstall:			; -$(RM) $(ALL)

####### (Details of Productions) ###############################################
bin:				; mkdir $@
$(MFILES):			; $(GET) $@
$B/$(THIS):	bin/$(THIS)	; $(PUT)
