# $Id: Makefile,v 11.3 1992/10/16 09:27:19 dickey Exp $
# Make-file for RCS baseline utility

THIS	= baseline
TOP	= ../../../..
include $(TOP)/cm_library/support/cm_library.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)
CSRC	= $(THIS).c
SOURCES	= $(CSRC)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)

clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_tests:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(THIS).o $(LIBS)	; $(CC) -o $@ $(THIS).o $(LIBS)

$(THIS).o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
lincnt.out:	$(CSRC)		; $(DATE); lincnt $(CSRC) >>$@
