# @(#)Makefile	1.1 88/05/21 10:09:17
# Make-file for SCCS/RCS checkin utilities
B	= ../../bin
I	= ../lib
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	=\
	../lib/lib.a
#
ALL	=\
	checkout\
	sccsdate
#
all:	$(ALL)
#
clean:
	rm *.out *.o
clobber: clean
	rm $(ALL)
#
SRC	=\
	checkout.c\
	sccsdate.c
#
checkout:	checkout.o $(LIBS)
	$(CC) -o $@ checkout.o $(LIBS)
#
sccsdate:	sccsdate.o $(LIBS)
	$(CC) -o $@ sccsdate.o $(LIBS)
#
checkout.o:	$I/ptypes.h	checkout.c
sccsdate.o:	$I/ptypes.h	sccsdate.c
#
# Install this stuff in public bin:
install:\
	$B/checkout\
	$B/rcsget\
	$B/sccsdate\
	$B/sccsget
#
$B/checkout:	checkout	; rm -f $@; cp $? $@; chmod +x $@
$B/rcsget:	rcsget.sh	; rm -f $@; cp $? $@; chmod +x $@
$B/sccsdate:	sccsdate	; rm -f $@; cp $? $@; chmod +x $@
$B/sccsget:	sccsget.sh	; rm -f $@; cp $? $@; chmod +x $@
#
# Development stuff:
lint.out:	$(SRC)
	echo '****'`date` | tee -a $@
	lintit.sh $? | tee -a $@
lincnt.out:	$(SRC)
	echo '**** '`date` | tee -a $@
	lincnt $(SRC) | tee -a $@
