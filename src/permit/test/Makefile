# $Id: Makefile,v 11.5 1994/11/09 00:16:26 tom Exp $
# make-file for testing RCS permission utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

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
	run_test.sh

SOURCES	= $(SCRIPTS) $(REF_FILES)

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
