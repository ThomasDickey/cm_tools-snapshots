# $Id: Makefile,v 11.4 1992/11/24 07:23:08 dickey Exp $
# make-file for testing RCS permission utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
REF_FILES = \
	delete.ref\
	insert.ref\
	unlock.ref
SCRIPTS	=\
	compare.sh \
	run_tests.sh

SOURCES	= $(SCRIPTS) $(REF_FILES)

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; rm -rf $(CLEAN) foo* FOO
clobber:	clean
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_tests:	$(SOURCES)	; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
