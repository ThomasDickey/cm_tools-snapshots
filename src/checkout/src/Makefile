# $Header: /users/source/archives/cm_tools.vcs/src/checkout/src/RCS/Makefile,v 1.8 1988/09/01 06:49:30 dickey Exp $
# Make-file for RCS checkout utilities
#
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
ALL	= checkout
SCRIPTS	= rcsget.sh
CSRC	= checkout.c
SRCS	= $(CSRC) $(SCRIPTS)
MAIN_O	= checkout.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber: 	clean		; rm -f $(ALL)
destroy:			; rm -f *
sources: 	$(SRCS)
#
install:	$B/checkout\
		$B/rcsget
#
####### (Details of Productions) ###############################################
checkout:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
checkout.o:	$I/ptypes.h	$I/rcsdefs.h
#
$B/checkout:	checkout	; copy -d $@
$B/rcsget:	rcsget.sh	; copy $? $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
