# $Id: Makefile,v 11.1 1992/08/05 14:31:26 dickey Exp $
# Make-file for RCS/vcs utility

####### (Environment) ##########################################################
THIS	= vcs
include ../../common.mk

####### (Standard Lists) #######################################################
ALL	= $B/$(THIS)

OBJS	= \
	$(THIS).o \
	vcs_delete.o \
	vcs_insert.o \
	vcs_unlock.o

C_SRC	= \
	$(THIS).c \
	vcs_delete.c \
	vcs_insert.c \
	vcs_unlock.c

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
$B/$(THIS):	$(OBJS) $(LIBS)	; $(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS):	$(H_SRC) $(RCSDEFS_H)

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@
lint.out:	$(C_SRC)	; $(DATE); tdlint $(LINTOPT) $(C_SRC) >>$@
lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@
