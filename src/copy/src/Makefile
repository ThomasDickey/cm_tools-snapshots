# $Id: Makefile,v 11.7 1994/11/09 00:04:04 tom Exp $
# MAKE-file for enhanced unix copy utility

THIS	= copy
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)
CSRC	= $(THIS).c
SOURCES	= Makefile $(CSRC)

####### (Standard Productions) #################################################
all\
install:	$B $(SOURCES) $(ALL)

clean:				; -$(RM) $(CLEAN)
clobber: 	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
run_test:
sources: 	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/$(THIS):	$(THIS).o $(LIB_DEPS)	; $(LINK) -o $@ $(THIS).o $(LIB_ARGS)
$(THIS).o:	$(PTYPES_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(CSRC)		; $(DATE); tdlint $(LINTOPTS) $? >>$@
