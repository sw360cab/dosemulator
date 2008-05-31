# The following are the flags to compile using the GCC compiler
CFLAGS = -g 
CC = gcc
LDFLAGS = -lm


INCLUDES =	parse.h
#
OBJECTS =  shell_base.c get_line.c parse.c md.c

all	: $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o shell_base $(OBJECTS) 


#
#
clean   :
	rm -f *.o
