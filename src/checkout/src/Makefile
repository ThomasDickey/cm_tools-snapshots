# @(#)Makefile	1.4 88/07/29 10:29:07
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
	checkout
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
	checkout.c
#
checkout:	checkout.o $(LIBS)
	$(CC) -o $@ checkout.o $(LIBS)
#
checkout.o:	$I/ptypes.h	checkout.c
#
# Install this stuff in public bin:
install:\
	$B/checkout\
	$B/rcsget
#
$B/checkout:	checkout	; rm -f $@; cp $? $@; chmod +x $@
$B/rcsget:	rcsget.sh	; rm -f $@; cp $? $@; chmod +x $@
#
lintit.sh\
rcsget.sh\
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
