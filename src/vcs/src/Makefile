# $Id: Makefile,v 11.5 1993/09/22 13:46:21 dickey Exp $
# Make-file for RCS/vcs utility

THIS	= vcs
TOP	= ../../../..
include $(TOP)/td_lib/support/td_lib.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)

OBJS	= \
	$(THIS).o \
	delete.o \
	insert.o \
	unlock.o

C_SRC	= \
	$(THIS).c \
	delete.c \
	insert.c \
	unlock.c

H_SRC	= \
	$(THIS).h

SOURCES	= $(C_SRC) $(H_SRC)

####### (Standard Productions) #################################################
all\
install:	$(SOURCES) $(ALL)

clean:				; rm -f $(CLEAN)
clobber:	clean		; rm -f $(ALL)
destroy:			; $(DESTROY)
run_tests:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/$(THIS):	$(OBJS) $(LIBS)	; $(LINK) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS):	$(H_SRC) $(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); tdlint $(LINTOPTS) $(C_SRC) >>$@
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
