# $Id: Makefile,v 11.0 1991/10/17 09:58:04 ste_cm Rel $
# Make-file for RCS/vcs utility
#
####### (Environment) ##########################################################
B	= ../bin
I	= ../../common/interface
L	= ../../common/lib
GET	= checkout

####### (Command-line Options) #################################################
INCLUDES= -I$I
CPP_OPTS= $(DEFINES) $(INCLUDES)

LIBS	= $L/lib.a

DATE	= echo '****'`date` >> $@
LINTOPT	= $(CPP_OPTS) -ltd

####### (Standard Lists) #######################################################
ALL	= $B/vcs

OBJS	= \
	vcs.o \
	vcs_delete.o \
	vcs_insert.o \
	vcs_unlock.o

C_SRC	= \
	vcs.c \
	vcs_delete.c \
	vcs_insert.c \
	vcs_unlock.c

H_SRC	= \
	vcs.h

SOURCES	= $(C_SRC) $(H_SRC)

####### (Standard Productions) #################################################
all:		sources $(ALL)
#
clean:				; rm -f *.o *.out *.bak core
clobber:	clean		; rm -f $(ALL)
destroy:
	sh -c 'for i in *;do case $$i in RCS);; *) rm -f $$i;;esac;done;exit 0'
run_tests:
sources:	$(SOURCES)

####### (Details of Productions) ###############################################
$B/vcs:		$(OBJS) $(LIBS)	; $(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS):	$(H_SRC) $I/ptypes.h $I/common.h $I/rcsdefs.h

####### (Development) ##########################################################
$(SOURCES):			; $(GET) $@

lint.out:	$(C_SRC)	; $(DATE); tdlint $(LINTOPT) $(C_SRC) >>$@

lincnt.out:	$(C_SRC)	; $(DATE); lincnt $(C_SRC) >>$@

.c.o:
	$(CC) $(CFLAGS) $(CPP_OPTS) -c $<
