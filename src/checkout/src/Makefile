# $Id: Makefile,v 4.0 1989/08/22 09:51:55 ste_cm Rel $
# Make-file for RCS checkout utilities
#
# $Log: Makefile,v $
# Revision 4.0  1989/08/22 09:51:55  ste_cm
# BASELINE Thu Aug 24 09:27:38 EDT 1989 -- support:navi_011(rel2)
#
# Revision 3.1  89/08/22  09:51:55  dickey
# corrected 'destroy' rule
# 
# Revision 3.0  89/03/28  12:00:50  ste_cm
# BASELINE Mon Jun 19 13:12:40 EDT 1989
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
ALL	= $B/checkout $B/rcsget
CSRC	= checkout.c
SCRIPTS	= rcsget.sh
SOURCES	= Makefile $(CSRC) $(SCRIPTS)
MAIN_O	= checkout.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		sources $(ALL)
#
clean:				; rm -f *.o *.out
clobber: 	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources: 	$(SOURCES)
#
install:	$B/checkout
#
####### (Details of Productions) ###############################################
$B/checkout:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
$B/rcsget:	rcsget.sh	; copy rcsget.sh $@
#
checkout.o:	$I/ptypes.h	$I/rcsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
