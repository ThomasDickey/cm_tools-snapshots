# $Id: Makefile,v 11.0 1992/02/11 13:18:09 ste_cm Rel $
# make-file for testing RCS checkin utility
#
####### (Development) ##########################################################
GET	= checkout

TOP	= ../../common
INCLUDES= -I$(TOP)/interface
CPP_OPTS= $(DEFINES) $(INCLUDES)
LIBS	= $(TOP)/lib/lib.a

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

SOURCES	= Makefile $(SCRIPTS) $(REF_FILES) README set_date.c
ALL	= set_date

####### (Standard Productions) #################################################
all:		$(SOURCES) $(ALL)
clean:				; rm -f *.bak *.log *.out core *.o
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
sources:	$(SOURCES)
lint.out:
lincnt.out:
run_tests:	$(SOURCES) $(ALL) ; sh -c '$@.sh 2>&1 | tee -a $@.out'

####### (Details of Productions) ###############################################
set_date:	set_date.c	; $(CC) -o $@ $(CPP_OPTS) $@.c $(LIBS)
$(SOURCES):			; $(GET) $@
