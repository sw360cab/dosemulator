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

///*
// * '-' and '_' are allowed for paths
// */
//int under_s(int ch)
//{
//	if (ch=='-' || ch =='_')
//		return TRUE;
//	else
//		return FALSE;
//}
//
///*
// * check for forbidden characters
// */
//int alpha_num(char *path, char *ch)
//{
//	int i,count=strlen(path);
//
//	//printf ("here %s::%d-----\n", path, count);
//
//	// if name is alphaneumeric or contains '-' or '_' is OK
//	for(i=0;i<strlen(path) && ( isalnum(path[i]) || under_s(path[i]) ); i++)
//	{
//		count--;
//		//printf ("cnt = %d -- %c--\n",count,path[i]);
//	}
//
//	if (count==0)
//		return TRUE;
//
//	else
//	{
//		strncpy(ch,path+i,1);
//		return FALSE;
//	}
//}


/*
 * look inside the passed path
 * check correctness and store in list
 * path name
 */
param* parse_path(char *str)
{
	int ct=0,count=0;
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

		p->name = (char *) malloc(sizeof(char)* count+1 );

		strncpy(p->name, str, count);

		// check correctness of temporay string -> relative path
		// TODO can have problems with last string of path
		if (!alpha_num(p->name,&c,1))
		{
			printf ("Unexpected char \'%c\'\n", c);
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
	}while ( strncmp( str, "\0",1)!=0 );

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
	int i,count;
	mode_t mode;
	int fd;
	
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
		stat(working_dir,&st);

		while(p!=NULL){
			//printf("%s to be parsed and type %d\n", p->name, p->type);

			dir=opendir(p->name);

			if (dir!=NULL && p->type==1) // path existing
			{
				printf ("Path already exists !\n");
				exit(1);
			}
			else if (dir!=NULL)
			{
				stat(p->name,&st); // parent inode 
				// 	---> changes only when descending in an existing dir
				// printf ("Path mode %d!\n", st.st_mode);
			}
			else if (dir==NULL)
			{
				if (mkdir(p->name, st.st_mode) < 0 )
				{
					printf ("Unable to create directory or you don't have permission to do so\n");
					exit(1);
				}
			}

			chdir(p->name);

			p=p->next;
		}

		//free(p);
		list=list->next;
		if (list!=NULL)
			path = (char *)realloc(path, sizeof(char)*strlen(list->name));
		// return to working directory
		chdir(working_dir);
	}
	printf ("done\n");
	/*
	 * dir=opendir(path);

	if (dir == NULL)
		printf ("Failed !\n");
	else
		printf ("Path already exist !\n");


	chdir(path);
	stat(argv[1],&st);

	dir=opendir(argv[2]);

	if (dir == NULL)
		mkdir(argv[2], st.st_mode);
	else
		printf ("already\n");
	 */
}
