# $Id: Makefile,v 11.3 1992/10/27 09:33:46 dickey Exp $
# make-file for testing RCS checkin utility

####### (Development) ##########################################################
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

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
	run_tests.sh

C_SRC	=\
	run_tool.c \
	set_date.c

PROGS	=\
	run_tool \
	set_date

SOURCES	= Makefile $(SCRIPTS) $(REF_FILES) README $(C_SRC)
ALL	= $(PROGS)

####### (Standard Productions) #################################################
all:		$(SOURCES) $(ALL)
clean:				; rm -f $(CLEAN); rm -rf junk
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_tests:	$(SOURCES) $(ALL) ; $(RUN_TESTS)

####### (Details of Productions) ###############################################
set_date:	set_date.c	; $(CC) -o $@ $(CPP_OPTS) $@.c $(LIBS)
run_tool:	run_tool.c	; $(CC) -o $@ $(CPP_OPTS) $@.c $(LIBS)
$(SOURCES):			; $(GET) $@
