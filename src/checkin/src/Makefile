# $Header: /users/source/archives/cm_tools.vcs/src/checkin/src/RCS/Makefile,v 3.0 1989/03/28 11:57:11 ste_cm Rel $
# Make-file for RCS checkin utilities
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 11:57:11  ste_cm
# BASELINE Mon Jun 19 13:07:30 EDT 1989
#
# Revision 2.0  89/03/28  11:57:11  ste_cm
# BASELINE Thu Apr  6 08:47:10 EDT 1989
# 
# Revision 1.12  89/03/28  11:57:11  dickey
# added 'run_tests' rule
# 
# Revision 1.11  89/03/27  11:31:25  dickey
# maintain 'rcsput' script in same directory as 'checkin' module.
# 
# Revision 1.10  89/03/24  16:31:54  dickey
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
ALL	= $B/checkin $B/rcsput
CSRC	= checkin.c
SCRIPTS	= rcsput.sh
SRCS	= $(CSRC) $(SCRIPTS)
MAIN_O	= checkin.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber:	clean		; rm -f $(ALL)
destroy:			; rm -f *
run_tests:
sources:	$(SRCS)
#
####### (Details of Productions) ###############################################
$B/checkin:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
$B/rcsput:	rcsput.sh	; copy rcsput.sh $@
#
checkin.o:	$I/ptypes.h $I/rcsdefs.h
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
