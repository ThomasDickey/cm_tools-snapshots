# $Id: Makefile,v 11.0 1992/02/17 15:18:36 ste_cm Rel $

####### (Development) ##########################################################
GET	= checkout

####### (Standard Lists) #######################################################
SOURCES	=\
	README\
	config_sheet.txt\
	release_notes.txt\
	reusable_parts.txt

####### (Standard Productions) #################################################
all:		$(SOURCES)
clean:		; rm -f *.bak *.log *.out core
clobber:	clean
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)
install:
deinstall:

####### (Details of Productions) ###############################################
$(SOURCES):	; $(GET) $@
