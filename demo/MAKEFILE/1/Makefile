#CROSSCOMPILE=/opt/arm-2011.09/bin/arm-none-linux-gnueabi-
#CROSSCOMPILE=/opt/gcc-linaro-2013.03/bin/arm-linux-gnueabihf-

EXEC=ttytest
OBJS=rs232.o main.o uart0_parse.o

#CC=$(CROSSCOMPILE)gcc


all: $(EXEC)
	cp $(EXEC) /tftpboot/lz/filesys-mbox/home/. -f

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)


clean:
	rm *.o $(EXEC) -f
