# $Id: Makefile,v 9.0 1990/04/30 09:43:40 ste_cm Rel $
# MAKE-file for RCS skeleton tree utility
#
# $Log: Makefile,v $
# Revision 9.0  1990/04/30 09:43:40  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
#	Revision 8.0  90/04/30  09:43:40  ste_cm
#	BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
#	
#	Revision 7.0  90/04/30  09:43:40  ste_cm
#	BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
#	
#	Revision 6.1  90/04/30  09:43:40  dickey
#	corrected typo in 'sources' rule
#	
#	Revision 6.0  90/03/05  11:15:49  ste_cm
#	BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
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
