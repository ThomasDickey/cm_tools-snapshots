# $Id: Makefile,v 5.0 1989/10/19 11:52:50 ste_cm Rel $
# Make-file for RCS checkout utilities
#
# $Log: Makefile,v $
# Revision 5.0  1989/10/19 11:52:50  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#
# Revision 4.2  89/10/19  11:52:50  dickey
# updated lint-rule (now two programs)
# 
# Revision 4.1  89/10/19  08:35:22  dickey
# recoded 'rcsget' from Bourne-shell to C-language
# 
# Revision 4.0  89/08/22  09:51:55  ste_cm
# BASELINE Thu Aug 24 09:27:38 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.1  89/08/22  09:51:55  dickey
# corrected 'destroy' rule
# 
# Revision 3.0  89/03/28  12:00:50  ste_cm
# BASELINE Mon Jun 19 13:12:40 EDT 1989
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
ALL	= \
	$B/checkout \
	$B/rcsget
CSRC	= \
	checkout.c \
	rcsget.c
SOURCES	= Makefile $(CSRC)
MAIN_O	= checkout.o $(LIBS)
#
####### (Standard Productions) #################################################
all:		sources $(ALL)
#
clean:				; rm -f *.o *.out
clobber: 	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources: 	$(SOURCES)
#
install:	$B/checkout
#
####### (Details of Productions) ###############################################
$B/checkout:	checkout.o $(LIBS)	; $(CC) -o $@ checkout.o $(LIBS)
$B/rcsget:	rcsget.o $(LIBS)	; $(CC) -o $@ rcsget.o $(LIBS)
#
checkout.o \
rcsget.o:	$I/ptypes.h	$I/rcsdefs.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)
	$(DATE)
	sh -c 'for i in $?;do tdlint $(LINTOPT) $$i >>$@;done'
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
