# $Id: Makefile,v 11.7 1995/01/29 00:40:06 tom Exp $
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
install:	$(SOURCES) $B $(ALL)

clean:				; -$(RM) $(CLEAN)
clobber:	clean		; -$(RM) $(ALL)
destroy:			; $(DESTROY)
run_test:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B:				; mkdir $@
$B/$(THIS):	$(OBJS) $(LIB_DEPS) ; $(LINK) $(CFLAGS) -o $@ $(OBJS) $(LIB_ARGS)

$(OBJS):	$(H_SRC) $(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); tdlint $(LINTOPTS) $(C_SRC) >>$@
