# $Header: /users/source/archives/cm_tools.vcs/src/checkup/src/RCS/Makefile,v 1.4 1988/09/08 16:26:25 dickey Exp $
# MAKE-file for RCS/SCCS tree display
####### (Environment) ##########################################################
B	= ../../bin
I	= ../lib
GET	= getdelta
#
####### (Command-line Options) #################################################
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	= ../lib/lib.a
DATE	= echo '****'`date` >> $@
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd
#
####### (Standard Lists) #######################################################
ALL	= checkup
CSRC	= checkup.c
SRCS	= $(CSRC)
MAIN_O	= checkup.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber:	clean		; rm -f $(ALL)
destroy:			; rm -f *
sources:	$(SRCS)
#
install:	$B/checkup
#
####### (Details of Productions) ###############################################
checkup:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
#
checkup.o:	$I/ptypes.h	$I/rcsdefs.h
#
$B/checkup:	checkup		; copy -d $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
