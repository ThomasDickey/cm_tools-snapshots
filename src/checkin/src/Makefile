# @(#)Makefile	1.2 88/05/20 07:45:31
# Make-file for SCCS/RCS checkin utilities
B	= ../../bin
I	= ../lib
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	=\
	../lib/lib.a
#
ALL	=\
	rcsbase\
	sccsbase
#
all:	$(ALL)
#
clean:
	rm *.out *.o
clobber: clean
	rm $(ALL)
#
SRC	=\
	rcsbase.c\
	sccsbase.c
#
rcsbase:	rcsbase.o
	$(CC) -o $@ rcsbase.o $(LIBS)
#
sccsbase:	sccsbase.o
	$(CC) -o $@ sccsbase.o $(LIBS)
#
rcsbase.o:	$I/ptypes.h	rcsbase.c
sccsbase.o:	$I/ptypes.h	sccsbase.c
#
# Install this stuff in public bin:
install:\
	$B/rcsbase\
	$B/rcsput\
	$B/sccsbase\
	$B/sccsput
#
$B/rcsbase:	rcsbase		; rm -f $@; cp $? $@; chmod +x $@
$B/rcsput:	rcsput.sh	; rm -f $@; cp $? $@; chmod +x $@
$B/sccsbase:	sccsbase	; rm -f $@; cp $? $@; chmod +x $@
$B/sccsput:	sccsput.sh	; rm -f $@; cp $? $@; chmod +x $@
#
# Development stuff:
lint.out:	$(SRC)
	echo '****'`date` | tee -a $@
	lintit.sh $? | tee -a $@
lincnt.out:	$(SRC)
	echo '**** '`date` | tee -a $@
	lincnt $(SRC) | tee -a $@
