# $Header: /users/source/archives/cm_tools.vcs/src/checkup/src/RCS/Makefile,v 3.0 1989/03/28 14:22:50 ste_cm Rel $
# MAKE-file for RCS/SCCS tree display
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 14:22:50  ste_cm
# BASELINE Mon Jun 19 13:17:34 EDT 1989
#
# Revision 2.0  89/03/28  14:22:50  ste_cm
# BASELINE Thu Apr  6 09:32:39 EDT 1989
# 
# Revision 1.6  89/03/28  14:22:50  dickey
# added 'run_tests' rule
# 
# Revision 1.5  89/03/24  13:41:47  dickey
# revised to standard CM-format
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
ALL	= $B/checkup
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
run_tests:
sources:	$(SRCS)
#
####### (Details of Productions) ###############################################
$B/checkup:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
checkup.o:	$I/ptypes.h	$I/rcsdefs.h
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
