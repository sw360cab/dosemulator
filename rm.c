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

#define BUF_MAX 1024

extern char *get_line();


void erase(char *pth)
	{
	if ( unlink(pth) == -1)
	   	{
	   		fprintf(stderr, "Unable to delete file %s\n",pth);
	   		exit(1);
	   	}
	printf("file deleted %s\n",pth);
	return;
	}

// check if file is a read_only file
int rd_only(char *pth)
	{
	if ( access(pth,R_OK)==0 && access(pth,W_OK)<0 && access(pth,X_OK)<0 )
		return 1;
	else
		return 0;
	}

// ask request for deleting file
int request(char *pth, int rd_only)
	{
	char *answ;
	
	while(1)
		{
		if (rd_only==1)
			printf ("Are you sure to delete Read_Only file %s ? [Yes or No]\n", pth);
		else
			printf ("Are you sure to delete %s ? [Yes or No]\n", pth);
		
		answ=get_line();
		if (strncmp(answ,"yes",3)==0 || strncmp(answ,"YES",3)==0 || strncmp(answ,"Yes",3)==0 )
			return 1;
		else if ( strncmp(answ,"y",1)==0 || strncmp(answ,"Y",1)==0 )
			return 1;
		else	// NO
			{
			if (strncmp(answ,"no",2)==0 || strncmp(answ,"NO",2)==0 || strncmp(answ,"No",2)==0)
				return 0;
			else if ( strncmp(answ,"n",1)==0 || strncmp(answ,"N",1)==0 )
				return 0;
			else 
				fprintf(stderr, "Type Yes or No !\n");
			}
		}
	}
	
int main(int argc,char **argv) 
	{ 
	
	char *path;
	int source_fd, dest_fd;
	int rd;
	struct stat st;

	int req=0;
	int read_only=1;
	int hidden=0;
	int ronly=0;
	int attrib=0;
	int sub_dir=0;
	
	if ( strcmp(argv[2],"\\P")==0 ) // req of deletion for each file
		req=1;
	else if ( strcmp(argv[2],"\\F")==0 ) // force deletion of read-only file
		read_only=0;
	else if ( strcmp(argv[2],"\\Q")==0 ) // delete everything without asking
		{
		req=0;
		read_only=0;
		}
	else if ( strcmp(argv[2],"\\S")==0 ) // look for file to be deleted in subdirectories
		sub_dir=1;
	else if ( strcmp(argv[2],"\\A")==0 ) // delete according to attributes
		{
		attrib=1;
		printf("attrib\n");
		if (strcmp(argv[3],":-R")==0) // delete all but readonly
				ronly=0;
		
		if (strncmp( (argv[2])+2,":H",2)==0) // delete only if hidden
			hidden=1;
		else if (strncmp( (argv[2])+3,":-H",3)==0) // delete all but hidden
			hidden=0;
		if (strncmp( (argv[2])+2,":R",2)==0) // delete only if readonly
			ronly=1;
		else if (strncmp( (argv[2])+3,":-R",3)==0) // delete all but readonly
			ronly=0;
		}
	
	path = (char *) malloc( sizeof(char)*strlen(argv[1])+1 );
	strcpy(path,argv[1]);
	
	if (sub_dir!=1) // don't do this check if need to look in sub dir
		{
		stat(path,&st);
		if (S_ISDIR(st.st_mode)) // skip if directory
			{
			fprintf(stderr, " %s is a directory, try to use DelTree\n",path);
			exit(1);
			}
		if ( (source_fd=open (path, O_RDONLY)) == -1)  // try to open file
			{
			fprintf(stderr, "Unable to find file %s or you don\'t have enough permission\n",path);
			exit(1);
			}

		if (st.st_nlink > 1)  // unlink wouldn't delete for sure the file, whether others open it meanwhile
			{
			fprintf(stderr, "Check that file %s is not open or used by another file\n",path);
			exit(1);
			}	
		}
   	
   	if ( req==1 && request(path,0) ) // ask for request in case
   		{
   		erase(path);
   		close(source_fd);
   		}
   	   	
   	else if(attrib==1)
   		{
   		
   		if (ronly==1) // only readdonly
   			{
   			if ( rd_only(path) )
   			   	{
   			   		erase(path);
   			   		close(source_fd);
   			   	}
   			}
   		else // everything but readonly
	   		{
	   			if ( !rd_only(path) )
	   			{
	   				erase(path);
	   				close(source_fd);
	   			}
	   		}
   		if (hidden==1) // only hidden
   		   	{
   			if ( strncmp(path,".",1)==0 )
   			   	{
   			   		erase(path);
   			   		close(source_fd);
   			   	}
   			}
   		else // everything but hidden
	   		{
	   			if ( strncmp(path,".",1)!=0 )
	   			{
	   				erase(path);
	   				close(source_fd);
	   			}
	   		}
   		
   		}
   	
   	else if (read_only==1) // ask only for read-only files
   		{
   		
   		if ( rd_only(path) )
   		 	{ 
   			if ( request(path,1) ) // ask for request in case
   				{
   				erase(path);
   				close(source_fd);
   			   	}
   		 	}
   		
   		else // just delete
   			{
   			erase(path);
   			close(source_fd);
   			}
   		
   		}
    		
   	else // just delete
   		{
   		//if subdir --> check file in subdir recursively
   		{
   		erase(path);
   		close(source_fd);
   		}
   		}
	}
