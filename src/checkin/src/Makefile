# $Id: Makefile,v 4.0 1989/08/22 10:20:45 ste_cm Rel $
# Make-file for RCS checkin utilities
#
# $Log: Makefile,v $
# Revision 4.0  1989/08/22 10:20:45  ste_cm
# BASELINE Thu Aug 24 09:23:45 EDT 1989 -- support:navi_011(rel2)
#
# Revision 3.1  89/08/22  10:20:45  dickey
# corrected 'destroy' rule
# 
# Revision 3.0  89/03/28  11:57:11  ste_cm
# BASELINE Mon Jun 19 13:07:30 EDT 1989
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
CSRC	= checkin.c
SCRIPTS	= rcsput.sh
SOURCES	= $(CSRC) $(SCRIPTS)
MAIN_O	= checkin.o $(LIBS)
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
$B/checkin:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
$B/rcsput:	rcsput.sh	; copy rcsput.sh $@
#
checkin.o:	$I/ptypes.h $I/rcsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
