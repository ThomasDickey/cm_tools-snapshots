# $Id: Makefile,v 11.0 1991/10/18 07:44:41 ste_cm Rel $
# Make-file for RCS checkin utilities
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../common/interface
L	= ../../common/lib
GET	= checkout
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CPP_OPTS= $(DEFINES) $(INCLUDES)

LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(CPP_OPTS) -ltd

####### (Standard Lists) #######################################################
ALL	= $B/checkin $B/rcsput
CSRC	= checkin.c rcsput.c
SOURCES	= $(CSRC)
#
####### (Standard Productions) #################################################
all:		sources $(ALL)
#
clean:				; rm -f *.o *.out *.bak core
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)
#
####### (Details of Productions) ###############################################
$B/checkin:	checkin.o $(LIBS)	; $(CC) -o $@ checkin.o $(LIBS)
$B/rcsput:	rcsput.o  $(LIBS)	; $(CC) -o $@ rcsput.o  $(LIBS)
#
checkin.o \
rcsput.o:	$I/ptypes.h $I/rcsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)
	$(DATE)
	sh -c 'for i in $?;do tdlint $(LINTOPT) $$i >>$@;done'
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@

.c.o:
	$(CC) $(CFLAGS) $(CPP_OPTS) -c $<
