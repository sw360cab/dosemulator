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


void cp(param **list)
	{ 
	char *buf;
	char *src, *dest;
	int source_fd, dest_fd;
	int rd;
	struct stat st;
	
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

	src = (char *) malloc(sizeof(char)*strlen((*list)->name)+1);
	strcpy(src,(*list)->name);
	
	(*list)=(*list)->next;
	dest = (char *) malloc(sizeof(char)*strlen((*list)->name)+1);
	strcpy(dest,(*list)->name);
	
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
