# $Id: Makefile,v 11.6 1994/11/09 00:17:33 tom Exp $
# make-file for testing RCS permission utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
REF_FILES = \
	delete.ref\
	insert.ref\
	unlock.ref
SCRIPTS	=\
	compare.sh \
	run_test.sh

SOURCES	= $(SCRIPTS) $(REF_FILES)

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; -$(RM) -r $(CLEAN) foo* FOO
clobber:	clean
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
run_test:	$(SOURCES)	; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
