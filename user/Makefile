# $Header: /users/source/archives/cm_tools.vcs/user/RCS/Makefile,v 3.0 1989/06/14 07:39:45 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 3.0  1989/06/14 07:39:45  ste_cm
# BASELINE Mon Jun 19 15:30:29 EDT 1989
#
#	Revision 2.1  89/06/14  07:39:45  dickey
#	corrected destroy/clobber/clean rules
#	
#	Revision 2.0  89/03/29  08:25:03  ste_cm
#	BASELINE Thu Apr  6 14:48:29 EDT 1989
#	

####### (Development) ##########################################################
INSTALL_PATH=/ste_site/ste/doc
B	= $(INSTALL_PATH)
GET	= checkout
PUT	= copy -v -d $@

####### (Standard Lists) #######################################################
ALL	=\
	$B/baseline.doc\
	$B/checkin.doc\
	$B/checkout.doc\
	$B/checkup.doc\
	$B/copy.doc\
	$B/ded.doc\
	$B/link2rcs.doc\
	$B/permit.doc\
	$B/rcsget.doc\
	$B/rcsput.doc
SOURCES	=\
	baseline.doc\
	checkin.doc\
	checkout.doc\
	checkup.doc\
	copy.doc\
	ded.doc\
	link2rcs.doc\
	permit.doc\
	rcsget.doc\
	rcsput.doc

####### (Standard Productions) #################################################
all:		; @echo '** use "make install"'
clean:			; rm -f *.doc_*
clobber:	clean	; rm -f \$$*.doc_*
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done'
run_tests:
sources:	$(SOURCES)
install:	$(ALL)
deinstall:	; rm -f $(ALL)

####### (Details of Productions) ###############################################
$B/baseline.doc:	baseline.doc	; $(PUT)
$B/checkin.doc:		checkin.doc	; $(PUT)
$B/checkout.doc:	checkout.doc	; $(PUT)
$B/checkup.doc:		checkup.doc	; $(PUT)
$B/copy.doc:		copy.doc	; $(PUT)
$B/ded.doc:		ded.doc		; $(PUT)
$B/link2rcs.doc:	link2rcs.doc	; $(PUT)
$B/permit.doc:		permit.doc	; $(PUT)
$B/rcsget.doc:		rcsget.doc	; $(PUT)
$B/rcsput.doc:		rcsput.doc	; $(PUT)

$(SOURCES):				; $(GET) $@
