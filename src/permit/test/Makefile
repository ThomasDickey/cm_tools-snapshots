# $Id: Makefile,v 11.3 1992/11/24 09:31:25 dickey Exp $
# make-file for testing RCS permission utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
REF_FILES =\
	case0.ref \
	case1.ref \
	case2.ref \
	case3.ref \
	case4.ref \
	case5.ref

SCRIPTS	=\
	case0.sh \
	case1.sh \
	case2.sh \
	case3.sh \
	case4.sh \
	case5.sh \
	run_tests.sh

SOURCES	= $(SCRIPTS) $(REF_FILES)

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
