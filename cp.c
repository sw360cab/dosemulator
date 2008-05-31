/*
 * authors: Sergio Matone & Antonio Vetrò
 * tutor: Marco Murciano
 * group: Politecnico di Torino - System Programming
 *
 ** ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and  limitations under the
 * License.
 *
 * ***** END LICENSE BLOCK ***** 
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

#define BUF_MAX 1024

void err ()
{
	fprintf(stderr, "Error, check argument number or type\n");
	exit(1);
}

void cp(param **list)
	{ 
	char *buf;
	char *src, *dest;
	int source_fd, dest_fd;
	int rd;
	struct stat st;
	
	int verify=0, override=0;
	int flag=0, cat=0, dir=0;
	
	/* // copy for pure text file
	 * 
	 * FILE *in, *out;
	 * in = fopen(“wordlist.txt”, “r”);
	 * out = fopen(“copy.txt”, “w”);
	 * while ((c = getc (in)) != EOF) {
	 * 	putc(c,out);
	 * }
	 * fclose(in);
	 */ 

	// parse string 
	
	// resolve path
	// chdir to path
	
	while ((*list)!=NULL)
	{;
		printf("---%s---\n", (*list)->name);
		if( (*list)->type==0) ) //path
		{
			if (flag==0) // source
			{
				flag=1;
				if ( concat((*list)->name) )  // files concatenated
				{
					create_source((*list)->name);
					cat=1;
				}
				else if ( chk_dir((*list)->name) ) // source is a directory
				{
					dir=1;
					
					if (chdir( (*list)->name ) != 0)
						err();
				}
				else if ( isfile((*list)->name) )
				{
					src = (char *) malloc(sizeof(char)*strlen((*list)->name)+1);
					strcpy(src,(*list)->name);
				}
				else 
					err();
				
			}
			else if (flag==1) // destination
			{
				flag=2;
				
				if ( isfile((*list)->name)  )  // files
				{
					if (dir==1)
					{
						fprintf(stderr, "Error, source is a directory and destination is a file\n");
						exit(1);
					}
					else
					{
						dest = (char *) malloc(sizeof(char)*strlen((*list)->name)+1);
						strcpy(dest,(*list)->name);
					}
				}
				else if ( chk_dir((*list)->name) ) // dest is a directory
				{
					if (dir==1)
					{
						if (chdir( (*list)->name ) != 0)
							err();
					}
					else
					{
						fprintf(stderr, "Error, source is a file and destination is a directory\n");
						exit(1);
					}
					
				}
				else
					err();
			}
			else
				err();	
		}
		

		else if (*list)->type==1) ) //opt
		{
			if ( strcmp((*list)->name,"\A") ||  strcmp((*list)->name,"\B") )
				;
			else if ( strcmp((*list)->name,"\V") )
				verify=1;
			else if ( strcmp((*list)->name,"\Y") )
				override=1;
			else if ( strcmp((*list)->name,"\-Y") )
				override=0;	
			else
			{
				fprintf(stderr, "%s - unknown option\n",(*list)->name );
				exit(1);
			}
		}
		(*list)= (*list)->next;
	}
	
	if (dir==1) // directory copy
		{
		}
	
	else // file copy
		{
	
	   	if ( (source_fd=open (src, O_RDONLY)) == -1)
	   		{
	   		fprintf(stderr, "Can't open source file %s\n",src);
	   		exit(1);
	   		}
	   		
	   	stat(src,&st);
	   	buf = (char *) malloc(sizeof(char)*st.st_size+1);
	   	  		 
	   	if ( (dest_fd=open (dest, O_CREAT | O_WRONLY, st.st_mode)) == -1)
	   		{
	   		fprintf(stderr, "Can't open dest file %s\n",dest);
	   		exit(1);
	   		}
	   	
	   	/*rd=read(source_fd, buf, st.st_size+1);
	   	printf("read %d bytes of size %ld %u \n", rd,st.st_size, st.st_mode);
	   	
	   	rd=write(dest_fd, buf, rd);
	   	printf("wrote %d bytes\n", rd);
	   	*/
	   	
	   	while ((rd = read( source_fd, buf, st.st_size+1)) > 0 )
	    	write( dest_fd, buf, rd );
	    	
	    free(buf);
	    free(src);
	    free(dest);
		close(source_fd);
		close(dest_fd);
		}
	}
