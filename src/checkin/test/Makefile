# $Id: Makefile,v 11.8 1993/04/27 10:38:05 dickey Exp $
# make-file for testing RCS checkin utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
REF_FILES=\
	case1_a.ref\
	case1_b.ref\
	case2_a.ref\
	case2_b.ref\
	case2_c.ref\
	case2_d.ref\
	case3_a.ref\
	case3_b.ref\
	case3_c.ref\
	case3_d.ref\
	case3_e.ref\
	case3_f.ref

SCRIPTS	=\
	admin.sh\
	case1.sh\
	case1_a.sh\
	case1_b.sh\
	case2.sh\
	case2_a.sh\
	case2_b.sh\
	case2_c.sh\
	case2_d.sh\
	case3.sh\
	case3_a.sh\
	case3_b.sh\
	case3_c.sh\
	case3_d.sh\
	case3_e.sh\
	case3_f.sh\
	clean.sh\
	run_test.sh

SOURCES	= Makefile $(SCRIPTS) $(REF_FILES) README
ALL	=

####### (Standard Productions) #################################################
all:		$(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN); rm -rf junk
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_test:	$(SOURCES) $(ALL) ; $(RUN_TESTS)

####### (Details of Productions) ###############################################
$(SOURCES):			; $(GET) $@
