# $Id: Makefile,v 11.3 1992/11/24 16:14:25 dickey Exp $
# make-file for testing RCS skeleton tree utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
REF_FILES =\
	case1.ref\
	case2.ref

SCRIPTS	=\
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
