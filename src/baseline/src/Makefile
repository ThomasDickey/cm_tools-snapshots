# $Id: Makefile,v 11.6 1994/11/08 23:46:59 tom Exp $
# Make-file for RCS baseline utility

THIS	= baseline
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)
CSRC	= $(THIS).c
SOURCES	= $(CSRC)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)

clean:				; -$(RM) $(CLEAN)
clobber:	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(THIS).o $(LIB_DEPS)	; $(LINK) -o $@ $(THIS).o $(LIB_ARGS)

$(THIS).o:	$(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
