# $Header: /users/source/archives/cm_tools.vcs/src/link2rcs/src/RCS/Makefile,v 3.0 1989/03/28 13:16:22 ste_cm Rel $
# MAKE-file for RCS skeleton tree utility
#
# $Log: Makefile,v $
# Revision 3.0  1989/03/28 13:16:22  ste_cm
# BASELINE Mon Jun 19 14:44:56 EDT 1989
#
#	Revision 2.0  89/03/28  13:16:22  ste_cm
#	BASELINE Thu Apr  6 13:37:00 EDT 1989
#	
#	Revision 1.2  89/03/28  13:16:22  dickey
#	add 'run_tests' rule
#	
#	Revision 1.1  89/03/27  09:06:46  dickey
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
ALL	= $B/link2rcs
SCRIPTS	= link2rcs.sh
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
$B/link2rcs:	link2rcs.sh	; copy link2rcs.sh $@
#
####### (Development) ##########################################################
$(SRCS):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPT) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
