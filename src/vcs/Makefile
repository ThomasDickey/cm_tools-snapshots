# $Id: Makefile,v 11.5 1995/01/29 00:39:58 tom Exp $
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
run_test::	$L/$(TD_LIB).a bin

all\
clean\
clobber\
destroy\
sources\
run_test\
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
