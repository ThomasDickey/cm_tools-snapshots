# $Id: Makefile,v 6.0 1990/03/05 11:15:49 ste_cm Rel $
# MAKE-file for RCS skeleton tree utility
#
# $Log: Makefile,v $
# Revision 6.0  1990/03/05 11:15:49  ste_cm
# BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
#
#	Revision 5.2  90/03/05  11:15:49  dickey
#	split CFLAGS from INCLUDES
#	
#	Revision 5.1  89/11/30  12:02:37  dickey
#	rewrote link2rcs as a C-language program
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

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $<
