
SHELL=/bin/bash

CFLAGS+=-Wall -O2
CFLAGS+=-DNDEBUG

CFLAGS+=-D_GNU_SOURCE -Iinclude
#CFLAGS+=-Wall -ggdb3 -O0


OBJS=main.o flashcp.o misc.o led.o NetParser.o NetIo.o process_cmd.o 
OBJS += rs232.o SerialIo.o
OBJS+=param.o
EXEC=centctrl
LIBS+=-lpthread -lrt -lm -lc


.PHONY:tags clean

all: $(EXEC2) $(EXEC) public.h
	$(STRIP) -s $(EXEC) -o $(EXEC).strip
	cp $(EXEC) /tftpboot/$(USER)/filesys-mbox/home/. -f

$(EXEC) : $(OBJS) $(LIBNET) public.h
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)


install:

tags:
	ctags *.[ch]

%.o : %.c
	$(CC) -pipe $(CFLAGS) -c -o $@ $<

clean:
	rm $(EXEC) $(OBJS) *.o -f;
	rm *.strip -f

distclean: clean
	rm tags -f;
