# The following are the flags to compile using the GCC compiler
CFLAGS = -g 
CC = gcc
LDFLAGS = -lm -Wall
# -Wall

INCLUDES =	parse.h resource.h
#
OBJECTS =  shell_base.c parse.c resource.c md.c cp.c rm.c helper.c cd.c more.c attrib.c cls.c date.c echo.c fc.c launch_exe.c dir.c find.c

all	: $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o shell_base $(OBJECTS) 

#
clean   :
	rm -f *.o
