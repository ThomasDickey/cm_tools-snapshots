# $Id: Makefile,v 11.4 1992/12/04 14:06:27 ste_cm Exp $
# make-file for testing RCS skeleton tree utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
REF_FILES =\
	case1.ref\
	case2.ref

SCRIPTS	=\
	case1.sh\
	case2.sh\
	list_tree.sh\
	run_tests.sh

SOURCES	= Makefile $(SCRIPTS) $(REF_FILES)

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; rm -f $(CLEAN)
clobber:	clean
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_tests:	$(SOURCES)	; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
