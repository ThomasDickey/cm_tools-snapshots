# $Id: Makefile,v 10.0 1991/11/12 12:30:02 ste_cm Rel $
#

####### (Development) ##########################################################
INSTALL_MAN=../../install_man
COPY	= cp -p
GET	= checkout
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

CAT_FILES =\
	baseline.cat\
	checkin.cat\
	checkout.cat\
	checkup.cat\
	copy.cat\
	ded.cat\
	link2rcs.cat\
	permit.cat\
	rcsget.cat\
	rcsput.cat\
	vcs.cat

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
all:		$(ALL)			; @echo '** use "make install"'
clean:
clobber:	clean			; rm -f *.out *.cat core
destroy:	clobber
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:	$(CAT_FILES)
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
