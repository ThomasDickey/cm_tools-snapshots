# $Id: Makefile,v 4.1 1989/09/06 13:23:24 dickey Exp $
# MAKE-file for RCS directory-level administrative utility
#
# $Log: Makefile,v $
# Revision 4.1  1989/09/06 13:23:24  dickey
# RCS_BASE
#
# 
####### (Environment) ##########################################################
B	= ../bin
I	= ../../common/interface
L	= ../../common/lib
GET	= checkout
#
####### (Command-line Options) #################################################
PROG	= vcs
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= $L/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= $B/$(PROG)
CSRC	= $(PROG).c
SOURCES	= $(CSRC)
MAIN_O	= $(PROG).o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)
#
####### (Details of Productions) ###############################################
$B/$(PROG):	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
$(PROG).o:	$I/ptypes.h	$I/rcsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
