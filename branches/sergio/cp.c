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
#include <dirent.h>
#include <fcntl.h>
#include "parse.h"

void err ()
{
	fprintf(stderr, "Error, check argument number or type or files name or permissions\n");
	exit(1);
}

// check if the source file is a concatenation --> contains '+'
int concat (char *files)
{
	struct stat st;
	
	if (strchr(files,'+')!=NULL && stat(files,&st)!=0 )
		return TRUE;
	else
		return FALSE;
}

// create the list of files in case of a concatetanation
param* create_source(char *files)
{
	int end, count=0;
	int source_fd;
	param *p;
	param *pt=NULL;

	do
	{
		while ( strncmp( files+count, "+",1)!=0 && strncmp( files+count,"\0",1)!=0 )
			count++ ;
		end=count;
		p=new_elem();

		p->name = (char *) malloc(sizeof(char)* (end) );
		strncpy(p->name, files, end);
		
		// try to open file to verify exsistance
		
		if ( (source_fd=open (p->name, O_RDONLY)) == -1)
			{
			fprintf(stderr, "Can't open source file %s\n",p->name);
			exit(1);
			}
		close(source_fd);
		
		printf("%s to be parsed\n", p->name);
		p->type = 0;
		p->next=NULL;

		insert_e(&pt,p);

		files+=count+1;
		count=0;
	}while ( strncmp( files, "\0",1)!=0 );

	return pt;	
}

void cp(param **list)
	{ 
	char *buf, *temp_path;
	char *src, *dest;
	int source_fd, dest_fd;
	int rd;
	param *files=NULL;
	struct stat st;
	struct dirent *ep;
	DIR *dp;
	
	int verify=0, override=0;
	int flag=0, cat=0, dir=0;
	
	if(list==NULL)
		{
		fprintf(stderr, "copy: missing file operand\n");
		fprintf(stderr, "Try \'help copy\' for more information\n");
		exit(1);
		}
	
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
	
	while ((*list)!=NULL)
	{
		printf("---%s---\n", (*list)->name);
		if ( (*list)->type==0 ) //path
		{
			// take info about file / dir
			stat((*list)->name,&st);
			
			if (flag==0) // source
			{
				flag=1;
				
				if ( concat((*list)->name) )  // files concatenated
				{
					cat=1;
					if ((files=create_source((*list)->name))==NULL)
						err();
				}
				else if ( S_ISDIR(st.st_mode) ) // source is a directory
				{
					dir=1;
					if (opendir( (*list)->name ) == NULL)
						err();
					src = (char *) malloc(sizeof(char)*strlen((*list)->name)+1);
					strcpy(src,(*list)->name);
				}
				else if ( S_ISREG(st.st_mode) ) // source is a file
				{
					src = (char *) malloc(sizeof(char)*strlen((*list)->name)+1);
					strcpy(src,(*list)->name);
					
					// try to open source file
				   	if ( (source_fd=open (src, O_RDONLY)) == -1)
				   		{
				   		fprintf(stderr, "Can't open source file %s\n",src);
				   		exit(1);
				   		}
				}
				else 
					err();
				
			}
			else if (flag==1) // destination
			{
				flag=2;
				
				if ( S_ISDIR(st.st_mode) ) // dest is a directory
				{
					if (dir==1)
					{
						if (opendir( (*list)->name ) == NULL)
							err();
						dest = (char *) malloc(sizeof(char)*strlen((*list)->name)+1);
						strcpy(dest,(*list)->name);
					}
					else
					{
						fprintf(stderr, "Error, source is a file and destination is a directory\n");
						exit(1);
					}
				}	
				else if ( S_ISREG(st.st_mode) )  // dest is a file
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
				else
					err();
			}
			else
				err();	
		}
		
		else if ( (*list)->type==1 ) //opt
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
		else
			err();
		
		(*list)= (*list)->next;
	} // end of while - parsing line of parameters
	
	if (dir==1) // directory copy
		{
		// copy all the files of a directory into the new directory		
		dp = opendir(src);
		temp_path = (char *) malloc( sizeof(char)*MAXPATH+1 );

		if (dp != NULL) 
			{
			while ( (ep = readdir(dp)) )
				{				
				// resource is a file
				if ( ep->d_type==8 && (strcmp(ep->d_name, ".") != 0 ) && (strcmp(ep->d_name, "..")!= 0))
					{	
					// source file
					strcpy(src, ep->d_name);
				   	stat(src,&st);
				   	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);
				   	  
					// create file in dest directory
					strcpy(temp_path,dest);
					
					//if the current dir doesn't finish with '/',
					//I should add it before appending the currente resource name
					if ( dest[strlen(dest)-1] != '/')
									strcat(temp_path, "/");				
					strcat(temp_path, ep->d_name);
					} 	
				
					if ( (dest_fd=open (temp_path, O_CREAT | O_WRONLY, st.st_mode)) == -1)
				   		{
				   		fprintf(stderr, "Can't open dest file %s\n",temp_path);
				   		exit(1);
				   		}
				   	
				   	while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
				    	write( dest_fd, buf, rd );
				    	
				    free(buf);
				    free(src);
				    free(dest);
					close(source_fd);
					close(dest_fd);
				}
			}
			free(temp_path);
		}
	
	else if (cat==1) // files concatented
	{
//		// for each file open, copy, close
		
		stat(files->name,&st);
		buf = (char *) malloc(sizeof(char)*BUF_MAX+1);
		if (src==NULL) // src not already allocated
			src = (char *) malloc(sizeof(char)*strlen(files->name)+1);
		if ( (dest_fd=open (dest, O_CREAT | O_WRONLY, st.st_mode)) == -1)
		{
			fprintf(stderr, "Can't open dest file %s\n",dest);
			exit(1);
		}
		while (files != NULL)
			{
			strcpy(src,files->name);
			
		   	if ( (source_fd=open (src, O_RDONLY)) == -1)
		   		{
		   		fprintf(stderr, "Can't open source file %s\n",src);
		   		exit(1);
		   		}
		   	
			while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
				write( dest_fd, buf, rd );
		   	
			close(source_fd);
			files=files->next;
			}
		free(buf);
		free(src);
		free(dest);
		close(dest_fd);	
		}
	else // file copy
		{	
	   	stat(src,&st);
	   	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);
	   	  		 
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
	   	
	   	while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
	    	write( dest_fd, buf, rd );
	    	
	    free(buf);
	    free(src);
	    free(dest);
		close(source_fd);
		close(dest_fd);
		}
	}