# $Id: Makefile,v 11.0 1991/10/18 08:05:24 ste_cm Rel $
# MAKE-file for RCS/SCCS tree display

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
ALL	= $B/checkup
CSRC	= checkup.c
SOURCES	= Makefile $(CSRC)
MAIN_O	= checkup.o $(LIBS)

####### (Standard Productions) #################################################
all:		sources $(ALL)

clean:				; rm -f *.o *.out *.bak core
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/checkup:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
checkup.o:	$I/ptypes.h	$I/rcsdefs.h

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@

.c.o:
	$(CC) $(CFLAGS) $(CPP_OPTS) -c $<
