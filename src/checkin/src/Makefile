# $Id: Makefile,v 5.0 1989/10/23 15:18:43 ste_cm Rel $
# Make-file for RCS checkin utilities
#
# $Log: Makefile,v $
# Revision 5.0  1989/10/23 15:18:43  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#
# Revision 4.1  89/10/23  15:18:43  dickey
# rewrote 'rcsput' from Bourne-shell to C-language
# 
# Revision 4.0  89/08/22  10:20:45  ste_cm
# BASELINE Thu Aug 24 09:23:45 EDT 1989 -- support:navi_011(rel2)
# 
####### (Environment) ##########################################################
B	= ../bin
I	= ../../common/interface
L	= ../../common/lib
GET	= checkout
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= $B/checkin $B/rcsput
CSRC	= checkin.c rcsput.c
SOURCES	= $(CSRC)
#
####### (Standard Productions) #################################################
all:		sources $(ALL)
#
clean:				; rm -f *.o *.out
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
