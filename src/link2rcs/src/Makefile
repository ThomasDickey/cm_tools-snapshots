# $Id: Makefile,v 5.1 1989/11/29 12:46:20 dickey Exp $
# MAKE-file for RCS skeleton tree utility
#
# $Log: Makefile,v $
# Revision 5.1  1989/11/29 12:46:20  dickey
# rewrote link2rcs as a C-language program
#
#	Revision 5.0  89/08/22  09:11:34  ste_cm
#	BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
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
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd -lapollo
#
####### (Standard Lists) #######################################################
ALL	= $B/link2rcs
CSRC	= link2rcs.c
SOURCES	= Makefile $(CSRC)
MAIN_O	= link2rcs.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		sources	$(ALL)
#
clean:				; rm -f *.o *.out
clobber: 	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources: 	$(SRCS)
#
####### (Details of Productions) ###############################################
$B/link2rcs:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
link2rcs.o:	$I/ptypes.h	$I/rcsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
