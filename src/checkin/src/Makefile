# @(#)Makefile	1.5 88/07/29 09:39:26
# Make-file for SCCS/RCS checkin utilities
B	= ../../bin
I	= ../lib
GET	= getdelta
#
INCLUDES= -I$I
CFLAGS  = $(DEFINES) $(INCLUDES) -O
LIBS	=\
	../lib/lib.a
#
ALL	=\
	checkin
#
all:	$(ALL)
#
clean:
	rm -f *.out *.o
clobber: clean
	rm -f $(ALL)
destroy:
	rm -f *
#
SRCS	=\
	checkin.c
#
checkin:	checkin.o $(LIBS)
	$(CC) -o $@ checkin.o $(LIBS)
#
checkin.o:	$I/ptypes.h	checkin.c
#
# Install this stuff in public bin:
install:\
	$B/checkin\
	$B/rcsput
#
$B/checkin:	checkin		; rm -f $@; cp $? $@; chmod +x $@
$B/rcsput:	rcsput.sh	; rm -f $@; cp $? $@; chmod +x $@
#
$(SRCS):
	$(GET) -s $@
#
# Development stuff:
lint.out:	$(SRCS)
	echo '****'`date` | tee -a $@
	lintit.sh $? | tee -a $@
lincnt.out:	$(SRCS)
	echo '**** '`date` | tee -a $@
	lincnt $(SRCS) | tee -a $@
