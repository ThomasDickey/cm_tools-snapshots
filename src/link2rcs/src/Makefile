# $Id: Makefile,v 9.1 1991/09/09 07:39:30 dickey Exp $
# MAKE-file for RCS skeleton tree utility
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../common/interface
L	= ../../common/lib
GET	= checkout
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  =
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
clean:				; rm -f *.o *.out *.bak core
clobber: 	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources: 	$(SOURCES)
#
####### (Details of Productions) ###############################################
$B/link2rcs:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
link2rcs.o:	$I/ptypes.h	$I/rcsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $<
