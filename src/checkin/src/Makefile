# @(#)Makefile	1.4 88/06/02 13:02:17
# Make-file for SCCS/RCS checkin utilities
B	= ../../bin
I	= ../lib
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	=\
	../lib/lib.a
#
ALL	=\
	checkin\
	sccsbase
#
all:	$(ALL)
#
clean:
	rm -f *.out *.o
clobber: clean
	rm -f $(ALL)
#
SRC	=\
	checkin.c\
	sccsbase.c
#
checkin:	checkin.o $(LIBS)
	$(CC) -o $@ checkin.o $(LIBS)
#
sccsbase:	sccsbase.o $(LIBS)
	$(CC) -o $@ sccsbase.o $(LIBS)
#
checkin.o:	$I/ptypes.h	checkin.c
sccsbase.o:	$I/ptypes.h	sccsbase.c
#
# Install this stuff in public bin:
install:\
	$B/checkin\
	$B/rcsput\
	$B/sccsbase\
	$B/sccsput
#
$B/checkin:	checkin		; rm -f $@; cp $? $@; chmod +x $@
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
