# $Header: /users/source/archives/cm_tools.vcs/src/baseline/src/RCS/Makefile,v 3.0 1989/03/28 11:46:23 ste_cm Rel $
# MAKE-file for RCS baseline utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 11:46:23  ste_cm
# BASELINE Mon Jun 19 13:02:34 EDT 1989
#
#	Revision 2.0  89/03/28  11:46:23  ste_cm
#	BASELINE Tue Apr  4 16:06:13 EDT 1989
#	
#	Revision 1.2  89/03/28  11:46:23  dickey
#	added rule for 'run_tests'
#	
#	Revision 1.1  89/03/27  09:05:42  dickey
#	Initial revision
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
SRCS	= $(SCRIPTS)
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
$B/baseline:	baseline.sh	; copy baseline.sh $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
