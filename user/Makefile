# $Id: Makefile,v 9.1 1991/09/24 13:11:57 dickey Exp $
#

####### (Development) ##########################################################
INSTALL_MAN=../../install_man
COPY	= cp -p
GET	= checkout
PUT	= rm -f $@; $(COPY) $? $@
THIS	= cm_tools

SECTION	= 1
PREFIX	= $(INSTALL_MAN)/man$(SECTION)/

.SUFFIXES:
.SUFFIXES: .man .cat
#
.man.cat:
	rm -f $@
	nroff -man $? >$@

####### (Standard Lists) #######################################################
IT	=\
	$(PREFIX)baseline.$(SECTION)\
	$(PREFIX)checkin.$(SECTION)\
	$(PREFIX)checkout.$(SECTION)\
	$(PREFIX)checkup.$(SECTION)\
	$(PREFIX)copy.$(SECTION)\
	$(PREFIX)ded.$(SECTION)\
	$(PREFIX)link2rcs.$(SECTION)\
	$(PREFIX)permit.$(SECTION)\
	$(PREFIX)rcsget.$(SECTION)\
	$(PREFIX)rcsput.$(SECTION)

ALL	=\
	baseline.cat\
	checkin.cat\
	checkout.cat\
	checkup.cat\
	copy.cat\
	ded.cat\
	link2rcs.cat\
	permit.cat\
	rcsget.cat\
	rcsput.cat

SOURCES	=\
	baseline.man\
	checkin.man\
	checkout.man\
	checkup.man\
	copy.man\
	ded.man\
	link2rcs.man\
	permit.man\
	rcsget.man\
	rcsput.man

####### (Standard Productions) #################################################
all:		$(ALL)			; @echo '** use "make install"'
clean:
clobber:	clean			; rm -f *.out *.cat core
destroy:	clobber
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)
install:	$(IT)
deinstall:		; rm -f $(IT)

####### (Details of Productions) ###############################################
$(PREFIX)baseline.$(SECTION):	baseline.man	; $(PUT)
$(PREFIX)checkin.$(SECTION):	checkin.man	; $(PUT)
$(PREFIX)checkout.$(SECTION):	checkout.man	; $(PUT)
$(PREFIX)checkup.$(SECTION):	checkup.man	; $(PUT)
$(PREFIX)copy.$(SECTION):	copy.man	; $(PUT)
$(PREFIX)ded.$(SECTION):	ded.man		; $(PUT)
$(PREFIX)link2rcs.$(SECTION):	link2rcs.man	; $(PUT)
$(PREFIX)permit.$(SECTION):	permit.man	; $(PUT)
$(PREFIX)rcsget.$(SECTION):	rcsget.man	; $(PUT)
$(PREFIX)rcsput.$(SECTION):	rcsput.man	; $(PUT)

$(SOURCES):				; $(GET) $@
