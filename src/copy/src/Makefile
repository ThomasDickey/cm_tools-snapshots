# $Header: /users/source/archives/cm_tools.vcs/src/copy/src/RCS/Makefile,v 3.0 1989/03/28 12:59:04 ste_cm Rel $
# MAKE-file for enhanced unix copy utility
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
LINTOPT	= $(DEFINES) $(INCLUDES) -ltd -lapollo
#
####### (Standard Lists) #######################################################
ALL	= $B/copy
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
run_tests:
sources: 	$(SRCS)
#
####### (Details of Productions) ###############################################
$B/copy:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
copy.o:		$I/ptypes.h
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
