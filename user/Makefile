# $Id: Makefile,v 11.0 1992/02/18 11:48:23 ste_cm Rel $

####### (Development) ##########################################################
INSTALL_MAN=../../install_man

GET	= checkout
COPY	= cp -p
PUT	= rm -f $@; $(COPY) $? $@

THIS	= cm_tools

USE	= man
SECTION	= 1
PREFIX	= $(INSTALL_MAN)/$(USE)$(SECTION)/

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
	$(PREFIX)rcsput.$(SECTION)\
	$(PREFIX)vcs.$(SECTION)

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
	rcsput.man\
	vcs.man

ALL	=

####### (Standard Productions) #################################################
all:		$(ALL)
clean:					; rm -f *.bak *.log *.out core
clobber:	clean			; rm -f *.cat
destroy:	clobber
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)
install:	$(IT)
deinstall:		; rm -f $(IT)

####### (Details of Productions) ###############################################
$(PREFIX)baseline.$(SECTION):	baseline.$(USE)	; $(PUT)
$(PREFIX)checkin.$(SECTION):	checkin.$(USE)	; $(PUT)
$(PREFIX)checkout.$(SECTION):	checkout.$(USE)	; $(PUT)
$(PREFIX)checkup.$(SECTION):	checkup.$(USE)	; $(PUT)
$(PREFIX)copy.$(SECTION):	copy.$(USE)	; $(PUT)
$(PREFIX)ded.$(SECTION):	ded.$(USE)	; $(PUT)
$(PREFIX)link2rcs.$(SECTION):	link2rcs.$(USE)	; $(PUT)
$(PREFIX)permit.$(SECTION):	permit.$(USE)	; $(PUT)
$(PREFIX)rcsget.$(SECTION):	rcsget.$(USE)	; $(PUT)
$(PREFIX)rcsput.$(SECTION):	rcsput.$(USE)	; $(PUT)
$(PREFIX)vcs.$(SECTION):	vcs.$(USE)	; $(PUT)

$(SOURCES):				; $(GET) $@
