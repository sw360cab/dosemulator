# The following are the flags to compile using the GCC compiler
CFLAGS = -g 
CC = gcc
LDFLAGS = -lm 
# -Wall

INCLUDES =	parse.h
#
OBJECTS =  shell_base.c parse.c md.c cp.c rm.c cd.c echo.c helper.c

all	: $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o shell_base $(OBJECTS) 

#
clean   :
	rm -f *.o
