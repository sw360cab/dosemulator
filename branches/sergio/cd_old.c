/*
 * 
 * * 
 * TODO: wrapper for both cd and chdir
 * TODO: menage duoble quotes in the path ""
*/
 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc,char **argv) 
   { 
		if (argc < 2)
		{  printf("Usage: %s <pathname>\n",argv[0]);
						 exit(1);
		}
		
		/*if (chdir(argv[1]) != 0)
			{ printf("Error in chdir\n");
					 exit(1);
			}*/
			
		if ( open (argv[1], O_RDONLY) == -1)
   		{
   		fprintf(stderr, "Can't open source file %s\n",argv[1]);
   		exit(1);
   		}
	
		system("pwd");
		pause();
	}
