# $Id: Makefile,v 11.0 1991/10/18 07:50:42 ste_cm Rel $
# Make-file for RCS checkout utilities

####### (Environment) ##########################################################
B	= ../bin
I	= ../../common/interface
L	= ../../common/lib
GET	= checkout

####### (Command-line Options) #################################################
INCLUDES= -I$I
CPP_OPTS= $(DEFINES) $(INCLUDES)

LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(CPP_OPTS) -ltd

####### (Standard Lists) #######################################################
ALL	= \
	$B/checkout \
	$B/rcsget
CSRC	= \
	checkout.c \
	rcsget.c
SOURCES	= Makefile $(CSRC)
MAIN_O	= checkout.o $(LIBS)

####### (Standard Productions) #################################################
all:		sources $(ALL)

clean:				; rm -f *.o *.out *.bak core
clobber: 	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources: 	$(SOURCES)

install:	$B/checkout

####### (Details of Productions) ###############################################
$B/checkout:	checkout.o $(LIBS)	; $(CC) -o $@ checkout.o $(LIBS)
$B/rcsget:	rcsget.o $(LIBS)	; $(CC) -o $@ rcsget.o $(LIBS)

checkout.o \
rcsget.o:	$I/ptypes.h	$I/rcsdefs.h

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)
	$(DATE)
	sh -c 'for i in $?;do tdlint $(LINTOPT) $$i >>$@;done'
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@

.c.o:
	$(CC) $(CFLAGS) $(CPP_OPTS) -c $<
