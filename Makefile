# The following are the flags to compile using the GCC compiler
CFLAGS = -g 
CC = gcc
LDFLAGS = -lm

#
OBJECTS =  get_line.c resource.c dir.c 

all	: $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o dir $(OBJECTS) 


#
#
clean   :
	rm -f *.o
