# $Header: /users/source/archives/cm_tools.vcs/src/permit/src/RCS/Makefile,v 3.0 1989/03/28 13:59:42 ste_cm Rel $
# MAKE-file for RCS directory-permissions utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 13:59:42  ste_cm
# BASELINE Mon Jun 19 14:49:13 EDT 1989
#
# Revision 2.0  89/03/28  13:59:42  ste_cm
# BASELINE Thu Apr  6 13:54:53 EDT 1989
# 
# Revision 1.4  89/03/28  13:59:42  dickey
# added 'run_tests' rule
# 
# Revision 1.3  89/03/23  10:25:31  dickey
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
ALL	= $B/permit
CSRC	= permit.c
SRCS	= $(CSRC)
MAIN_O	= permit.o $(LIBS)
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
$B/permit:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
permit.o:	$I/ptypes.h	$I/rcsdefs.h
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
