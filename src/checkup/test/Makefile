# $Id: Makefile,v 11.1 1992/08/07 14:46:07 dickey Exp $
# make-file for testing RCS/sccs checkup utility

####### (Development) ##########################################################
include ../../common.mk

####### (Standard Lists) #######################################################
SCRIPTS	=\
	run_tests.sh

SOURCES	= Makefile $(SCRIPTS)

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:				; rm -f $(CLEAN)
clobber:	clean
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_tests:	$(SOURCES)	; sh -c '$@.sh 2>&1 | tee -a $@.out'

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
