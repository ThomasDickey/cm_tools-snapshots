# $Id: Makefile,v 11.3 1992/10/16 09:26:14 dickey Exp $
# MAKE-file for enhanced unix copy utility

THIS	= copy
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)
CSRC	= $(THIS).c
SOURCES	= Makefile $(CSRC)
MAIN_O	= $(THIS).o $(LIBS)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)

clean:				; rm -f $(CLEAN)
clobber: 	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_tests:
sources: 	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(MAIN_O)	; $(CC) -o $@ $(MAIN_O)
$(THIS).o:	$(PTYPES_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
