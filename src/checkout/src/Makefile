# $Header: /users/source/archives/cm_tools.vcs/src/checkout/src/RCS/Makefile,v 3.0 1989/03/28 12:00:50 ste_cm Rel $
# Make-file for RCS checkout utilities
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 12:00:50  ste_cm
# BASELINE Mon Jun 19 13:12:40 EDT 1989
#
# Revision 2.0  89/03/28  12:00:50  ste_cm
# BASELINE Thu Apr  6 09:22:38 EDT 1989
# 
# Revision 1.11  89/03/28  12:00:50  dickey
# added 'run_tests' rule.
# 
# Revision 1.10  89/03/27  11:33:57  dickey
# maintain 'rcsget' script in same directory as 'checkout' module.
# 
# Revision 1.9  89/03/24  15:39:52  dickey
# revised to CM-standard format
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
SRCS	= $(CSRC) $(SCRIPTS)
MAIN_O	= checkout.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber: 	clean		; rm -f $(ALL)
destroy:			; rm -f *
run_tests:
sources: 	$(SRCS)
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
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
