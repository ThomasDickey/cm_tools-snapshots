# @(#)Makefile	1.1 88/05/19 10:55:15
# Make-file for SCCS/RCS checkin utilities
B	= ../../bin
I	= ../lib
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	=\
	../lib/lib.a
#
it:	\
	rcsbase\
	sccsbase
#
rcsbase:	rcsbase.o
	$(CC) -o $@ rcsbase.o $(LIBS)
#
sccsbase:	sccsbase.o
	$(CC) -o $@ sccsbase.o $(LIBS)
#
rcsbase.o:	$I/ptypes.h	rcsbase.c
sccsbase.o:	$I/ptypes.h	sccsbase.c
