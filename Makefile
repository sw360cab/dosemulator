# The following are the flags to compile using the GCC compiler
CFLAGS = -g 
CC = gcc
LDFLAGS = -lm

#
INCLUDES = resource.h parse.h
OBJECTS =  resource.c dir.c find.c shell_base.c get_line.c parse.c cp.c cd.c more.c attrib.c

all	: $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o shell_base $(OBJECTS) 


#
#
clean   :
	rm -f *.o
