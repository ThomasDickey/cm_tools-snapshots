# $Header: /users/source/archives/cm_tools.vcs/src/copy/src/RCS/Makefile,v 1.4 1988/08/23 07:49:23 dickey Exp $
# MAKE-file for enhanced unix copy utility
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
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd -lapollo
#
####### (Standard Lists) #######################################################
ALL	= copy
CSRC	= copy.c
SRCS	= $(CSRC)
MAIN_O	= copy.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		$(ALL)
#
clean:				; rm -f *.o *.out
clobber: 	clean		; rm -f $(ALL)
destroy:			; rm -f *
sources: 	$(SRCS)
#
install:	$B/copy
#
####### (Details of Productions) ###############################################
copy:	$(MAIN_O)		; $(CC) -o $@ $(MAIN_O)
copy.o:	$I/ptypes.h
#
$B/copy:	copy		; copy -d $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
