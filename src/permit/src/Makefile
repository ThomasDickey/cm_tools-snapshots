# $Id: Makefile,v 5.0 1989/08/22 09:03:11 ste_cm Rel $
# MAKE-file for RCS directory-permissions utility
#
# $Log: Makefile,v $
# Revision 5.0  1989/08/22 09:03:11  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#
# Revision 4.0  89/08/22  09:03:11  ste_cm
# BASELINE Thu Aug 24 10:40:29 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.1  89/08/22  09:03:11  dickey
# corrected 'destroy' rule
# 
# Revision 3.0  89/03/28  13:59:42  ste_cm
# BASELINE Mon Jun 19 14:49:13 EDT 1989
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
ALL	= $B/permit
CSRC	= permit.c
SRCS	= $(CSRC)
MAIN_O	= permit.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SRCS)
#
####### (Details of Productions) ###############################################
$B/permit:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
permit.o:	$I/ptypes.h	$I/rcsdefs.h
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
