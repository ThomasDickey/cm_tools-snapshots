# $Id: Makefile,v 11.1 1992/08/07 14:46:41 dickey Exp $
# make-file for testing RCS permission utility

####### (Development) ##########################################################
include ../../common.mk

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
run_tests:	$(SOURCES)	; sh -c '$@.sh 2>&1 | tee -a $@.out'

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
