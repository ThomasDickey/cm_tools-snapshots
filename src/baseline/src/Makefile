# $Id: Makefile,v 4.0 1989/08/22 10:28:11 ste_cm Rel $
# MAKE-file for RCS baseline utility
#
# $Log: Makefile,v $
# Revision 4.0  1989/08/22 10:28:11  ste_cm
# BASELINE Thu Aug 24 09:20:00 EDT 1989 -- support:navi_011(rel2)
#
#	Revision 3.1  89/08/22  10:28:11  dickey
#	corrected 'destroy' rule; patched out 'lint.out' & 'lincnt.out'
#	
#	Revision 3.0  89/03/28  11:46:23  ste_cm
#	BASELINE Mon Jun 19 13:02:34 EDT 1989
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
ALL	= $B/baseline
SCRIPTS	= baseline.sh
SOURCES	= $(SCRIPTS)
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
$B/baseline:	baseline.sh	; copy baseline.sh $@
#
####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out: #	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out: #	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
