#CROSSCOMPILE=/opt/arm-2011.09/bin/arm-none-linux-gnueabi-
#CROSSCOMPILE=/opt/gcc-linaro-2013.03/bin/arm-linux-gnueabihf-

CC=/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-fsl-linux-gnueabi-gcc

EXEC=ttytest
OBJS=rs232.o main.o param.o SerialIo.o misc.o process_cmd.o NetParser.o NetIo.o 
LIBS+=-lpthread -lrt -lm -lc
#CC=$(CROSSCOMPILE)gcc


all: $(EXEC)
	cp $(EXEC) /tftpboot/lz/filesys-mbox/home/. -f

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -static


clean:
	rm *.o $(EXEC) -f
