# $Id: Makefile,v 5.0 1989/10/24 12:53:18 ste_cm Rel $
# Make-file for RCS baseline utility
#
# $Log: Makefile,v $
# Revision 5.0  1989/10/24 12:53:18  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#
#	Revision 4.1  89/10/24  12:53:18  dickey
#	rewrote 'baseline' into C-language
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
ALL	= $B/baseline
CSRC	= baseline.c
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
$B/baseline:	baseline.o $(LIBS)	; $(CC) -o $@ baseline.o $(LIBS)
#
baseline.o:	$I/ptypes.h $I/rcsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)
	$(DATE)
	sh -c 'for i in $?;do tdlint $(LINTOPT) $$i >>$@;done'
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
