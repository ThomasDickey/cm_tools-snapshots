# $Id: Makefile,v 5.0 1989/08/22 09:27:23 ste_cm Rel $
# MAKE-file for enhanced unix copy utility
#
# $Log: Makefile,v $
# Revision 5.0  1989/08/22 09:27:23  ste_cm
# BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#
# Revision 4.0  89/08/22  09:27:23  ste_cm
# BASELINE Thu Aug 24 10:16:23 EDT 1989 -- support:navi_011(rel2)
# 
# Revision 3.1  89/08/22  09:27:23  dickey
# corrected 'destroy' rule
# 
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
SOURCES	= Makefile $(CSRC)
MAIN_O	= copy.o $(LIBS)
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
####### (Details of Productions) ###############################################
$B/copy:	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
copy.o:		$I/ptypes.h
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
