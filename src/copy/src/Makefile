# $Id: Makefile,v 11.6 1994/06/18 19:34:50 tom Exp $
# MAKE-file for enhanced unix copy utility

THIS	= copy
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)
CSRC	= $(THIS).c
SOURCES	= Makefile $(CSRC)
MAIN_O	= $(THIS).o $(LIBS)

####### (Standard Productions) #################################################
all\
install:	$B $(SOURCES) $(ALL)

clean:				; rm -f $(CLEAN)
clobber: 	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_test:
sources: 	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/$(THIS):	$(MAIN_O)	; $(LINK) -o $@ $(MAIN_O)
$(THIS).o:	$(PTYPES_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
