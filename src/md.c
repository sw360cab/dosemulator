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
#include <dirent.h>
#include "parse.h"

// PROTOTYPES

/*
 * look inside the passed path
 * check correctness and store in list
 * path name
 * *** receive path - return list with parsed path ***
 */
param* parse_path(char *);

/*
 * md function
 * 1 - parse line
 * 2 - following path enters directories
 * 3 - if directories don't exist 
 * 		try to create it with same rights of parent directory
 * *** receive list with path and options ***
 */
void md(param *);



/*
 * look inside the passed path
 * check correctness and store in list
 * path name
 */
param* parse_path(char *str)
{
	int count=0;
	char c;
	param *p;
	param *pt=NULL;

	if (strncmp( str, "/",1)==0) // starting from root of filesystem
	{
		p=new_elem();

		p->name = (char *) malloc(sizeof(char));
		strncpy(p->name, str, 1);
		p->type = 0;
		p->next=NULL;

		insert_e(&pt,p);
		str++;
	}

	do
	{
		while ( strncmp( str+count, "/",1)!=0 && strncmp( str+count,"\0",1)!=0 )
			count++ ;

		p=new_elem();
		//printf ("parse_dir ++%s++\n", str);
		p->name = (char *) malloc(sizeof(char)* count+1 );
		strncpy(p->name, str, count);

		// check correctness of temporay string -> relative path
		if (!alpha_num(p->name,&c,1))
		{
			fprintf (stdout,"Unexpected char \'%c\'\n", c);
			exit(1);
		}

		if (strncmp( str+count,"\0",1)==0 )
			p->type = 1;
		else 
			p->type = 0;
		p->next=NULL;

		insert_e(&pt,p);

		str+=count+1;

		count=0;
	}while ( strncmp( str-1, "\0",1)!=0 && strncmp( str, "\0",1)!=0);

	return pt;	
}

/*
 * md function
 * 1 - parse line
 * 2 - following path enters directories
 * 3 - if directories don't exist 
 * 		try to create it with same rights of parent directory
 */
void md(param *list)	
{
	char *path, *working_dir;
	DIR *dir;
	struct stat st;
	param *p;
	mode_t st_mode, mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH ;
	int stat_res;

	if(list==NULL)
	{
		fprintf(stderr, "md: missing file operand\n");
		fprintf(stderr, "Try \'help md\' for more information\n");
		exit(1);
	}

	// working directory
	working_dir =(char *) malloc(sizeof(char)*MAXPATH);
	getcwd(working_dir,MAXPATH);

	path = (char *) malloc( sizeof(char)*strlen(list->name) );
	while (list!=NULL)
	{
		strcpy(path, list->name);

		p=parse_path(path);

		// get permission of working directory
		stat_res = stat(working_dir,&st);

		while(p!=NULL){
			//fprintf(stdout,"%s to be parsed and type %d\n", p->name, p->type);

			dir=opendir(p->name);

			if (dir!=NULL && p->type==1) // path existing
			{
				fprintf (stderr,"Cannot create path. Path already exists !\n");
				exit(1);
			}
			else if (dir!=NULL) // parent directory existing
			{
				stat_res = stat(p->name,&st); // parent inode 
				// 	---> changes only when descending in an existing dir
				//fprintf (stdout,"Path OK !\n");
			}
			else if (dir==NULL) // create directory from scratch
			{
				if (stat_res >= 0) // previus stat result ok
					st_mode = st.st_mode;
				else 
					st_mode = mode;

				if (mkdir(p->name, st_mode) < 0 )
				{
					fprintf (stdout,"Unable to create directory or you don't have permission to do so\n");
					exit(1);
				}
			}

			chdir(p->name);

			p=p->next;
		}

		my_free(&p);
		list=list->next;

		// return to working directory
		chdir(working_dir);
	}
	//fprintf (stdout,"done\n");
}
