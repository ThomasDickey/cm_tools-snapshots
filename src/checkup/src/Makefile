# @(#)Makefile	1.1 88/08/31 07:24:21
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
SCRIPTS	= test.sh
SRCS	= $(CSRC) $(SCRIPTS)
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
checkup.o:	$I/ptypes.h
#
$B/checkup:	checkup		; copy -d $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
