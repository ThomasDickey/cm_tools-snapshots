# @(#)Makefile	1.7 88/08/17 09:31:35
# Make-file for RCS checkin utilities
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
ALL	= checkin
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
sources:	$(SRCS)
#
install:	$B/checkin\
		$B/rcsput
#
####### (Details of Productions) ###############################################
checkin:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
checkin.o:	$I/ptypes.h
#
$B/checkin:	checkin		; copy -d $@
$B/rcsput:	rcsput.sh	; copy $? $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) -s sccs/s.$@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
