# $Id: Makefile,v 9.0 1989/08/22 08:38:29 ste_cm Rel $
#
# $Log: Makefile,v $
# Revision 9.0  1989/08/22 08:38:29  ste_cm
# BASELINE Mon Jun 10 10:09:56 1991 -- apollo sr10.3
#
#	Revision 8.0  89/08/22  08:38:29  ste_cm
#	BASELINE Mon Aug 13 15:06:41 1990 -- LINCNT, ADA_TRANS
#	
#	Revision 7.0  89/08/22  08:38:29  ste_cm
#	BASELINE Mon Apr 30 09:54:01 1990 -- (CPROTO)
#	
#	Revision 6.0  89/08/22  08:38:29  ste_cm
#	BASELINE Thu Mar 29 07:37:55 1990 -- maintenance release (SYNTHESIS)
#	
#	Revision 5.0  89/08/22  08:38:29  ste_cm
#	BASELINE Fri Oct 27 12:27:25 1989 -- apollo SR10.1 mods + ADA_PITS 4.0
#	
#	Revision 4.0  89/08/22  08:38:29  ste_cm
#	BASELINE Thu Aug 24 11:13:33 EDT 1989 -- support:navi_011(rel2)
#	
#	Revision 3.1  89/08/22  08:38:29  dickey
#	added 'sources' dependency to 'all'
#	
#	Revision 3.0  89/06/14  07:39:45  ste_cm
#	BASELINE Mon Jun 19 15:30:29 EDT 1989
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
all:		sources	; @echo '** use "make install"'
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
