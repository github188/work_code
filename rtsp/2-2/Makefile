
#tool chain
#CROSS = arm-hisiv100-linux-
CC = $(CROSS)gcc

#compile parameters
CFLAGS += -Wall -o2
CLINKS += -lpthread -lm
COMPILE.c = $(CC) $(CFLAGS) -c
LINK.c = $(CC) $(CLINKS)

#target source
SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(SRC:%.c=%.o)

TARGET = test

.PHONY: clean all
	all: $(TARGET)

$(TARGET): $(OBJ)
	    @echo
		    @echo Linking $@ from $^..
			    @$(LINK.c) -o $@ $^ -lpthread 

$(OBJ): %.o : %.c
	    @echo Compiling $@ from $<..
		    @$(COMPILE.c) -o $@ $^
			    
clean:
	    @rm -f $(TARGET)
		    @rm -f $(OBJ)


