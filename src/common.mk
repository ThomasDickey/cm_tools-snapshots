# $Id: common.mk,v 11.3 1992/08/07 14:09:48 dickey Exp $
# common definitions for makefiles built over CM_TOOLS library.

####### (Environment) ##########################################################
THAT	= common
TOP	= ../..

B	= ../bin
I	= $(TOP)/$(THAT)/interface
J	= $(TOP)/../interface
L	= $(TOP)/$(THAT)/lib

GET	= checkout
COPY	= cp -p
PUT	= rm -f $@; $(COPY) $? $@

MAKE	= make $(MFLAGS) -k$(MAKEFLAGS)	CFLAGS="$(CFLAGS)" COPY="$(COPY)"

####### (Command-line Options) #################################################
INCLUDES= -I. -I$I -I$J
CPP_OPTS= $(DEFINES) $(INCLUDES)

LIBS	= $L/$(THAT).a
DATE	= echo '** '`date` >> $@
LINTOPT	= $(CPP_OPTS) -ltd -lcurses -lapollo

####### (Standard Lists) #######################################################
CLEAN	= *.[oai] *.bak *.log *.out *.tst .nfs* core
DESTROY	=sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'

PTYPES_H =	$I/ptypes.h	$I/common.h
RCSDEFS_H =	$(PTYPES_H)	$I/rcsdefs.h	$I/deltree.h
SCCSDEFS_H =	$(PTYPES_H)	$I/sccsdefs.h

####### (Development) ##########################################################
CPROTO	= cproto -e -i -fo'\\n\\t\\t' -fp'\\n\\t\\t'

LINT_EACH = sh -c 'for i in $?;do echo $$i:; tdlint $(LINTOPT) $$i >>$@;done'

.SUFFIXES: .c .i .o .a .proto .lint .tst

.c.o:		; $(CC) $(CPP_OPTS) $(CFLAGS) -c $<
.c.a:
	@echo compile $<
	@$(CC) $(CFLAGS) $(CPP_OPTS) -c $*.c
	@ar rv $@ $*.o
	@rm -f $*.o
.c.i:		; $(CC) $(CPP_OPTS) -E -C $< >$@
.c.proto:	; $(CPROTO) $(CPP_OPTS) $< >$@
.c.lint:	; tdlint $(LINTOPT) $< >$@

.c.tst:		; $(CC) -o $@ -DTEST $(CFLAGS) $(CPP_OPTS) $< $Z $(LIBS)
