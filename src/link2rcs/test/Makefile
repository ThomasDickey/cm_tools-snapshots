# $Id: Makefile,v 11.6 1994/11/09 00:16:26 tom Exp $
# make-file for testing RCS skeleton tree utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
REF_FILES =\
	case1.ref\
	case2.ref

SCRIPTS	=\
	listtree.sh\
	run_test.sh

SOURCES	= Makefile $(SCRIPTS) $(REF_FILES)

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; -$(RM) $(CLEAN)
clobber:	clean
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
run_test:	$(SOURCES)	; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
