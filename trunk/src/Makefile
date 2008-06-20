CFLAGS = -g 
CC = gcc
LDFLAGS = -lm -Wall
# -Wall

SRC := $(shell find . -name "*.c")
OBJS := $(shell find . -name "*.c" | sed 's/\.c/\.o/')
HDRS := $(shell find . -name "*.h")
TARGET := shell_base

default: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(TARGET) $(OBJS)

.c.o: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@
       
#
clean:
	rm -f *.o
