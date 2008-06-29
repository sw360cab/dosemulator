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

// PROTOTYPES

// simply try o delete passed file
// *** receive path name ***
void erase(char *);

// check if file is a read_only file
// *** receive path name ***
int rd_only(char *);

// request for deleting file
// *** receive path name, 1 for read only / 0 else - return 1 -> Yes / 0 -> No ***
int request(char *, int);

/*
 * recursive function that enter recursively
 * directories, delete files and at the end delete 
 * also the specified directory --> rm -Rf
 * *** receive path name, ask flag ***
 */
void recur_del(char *, int);

/*
 * recursive function that enter recursively
 * directories, delete files if they have specified name
 * *** receive path name, file name  *** 
 */
void recur_subdir(char *, char *);

// del command with various options to delete given files
// *** receive list with path and options ***
void del(param *);

// deltree command --> rm -Rf
// *** receive list with path and options ***
void deltree(param *);

// rmdir
// *** receive list with path and options ***
void rd(param *);


int req=0;

/*
 * simply try o delete passed file
 */
void erase(char *pth)
{
	// ask for request in case
	if ( req==0 || (req==1 && request(pth,0)) ) 
	{
		if ( unlink(pth) == -1)
		{
			fprintf(stderr, "Unable to delete file %s\n",pth);
			exit(1);
		}
	}
	//fprintf(stdout,"file deleted %s\n",pth);
	return;
}

/*
 *  check if file is a read_only file
 */
int rd_only(char *pth)
{
	if ( access(pth,R_OK)==0 && access(pth,W_OK)<0 && access(pth,X_OK)<0 )
		return 1;
	else
		return 0;
}

/*
 * request for deleting file
 */
int request(char *pth, int rd_only)
{
	char *answ;

	while(1)
	{
		if (rd_only==1)
			fprintf(stdout,"Are you sure to delete Read_Only file %s ? [Yes or No]\n", pth);
		else
			fprintf(stdout,"Are you sure to delete %s ? [Yes or No]\n", pth);

		answ=get_line();


		if (strncasecmp(answ,"yes",3)==0 || strncasecmp(answ,"y",1)==0 || strncmp(answ,"Yes",3)==0 )
			return 1;
		else	
		{	// NO
			if ( strncasecmp(answ,"no",3)==0 || strncasecmp(answ,"n",1)==0 || strncmp(answ,"No",3)==0 )
				return 0;
			else 
				fprintf(stderr, "Type Yes or No!\n");
		}
	}
}

/*
 * recursive function that enter recursively
 * directories, delete files and at the end delete 
 * also the specified directory --> rm -Rf
 */
void recur_del(char *current_path, int ask)
{
	DIR *dp;
	struct dirent *ep;
	char *temp_path;

	temp_path = (char *) malloc( sizeof(char)*MAXPATH+1 );

	dp = opendir(current_path);

	if (dp != NULL) {
		while ( (ep = readdir(dp)) )
		{
			strcpy(temp_path,current_path);

			//if the current dir doesn't finish with '/',
			//I should add it before appending the currente resource name
			if ( current_path[strlen(current_path)-1] != '/')
				strcat(temp_path, "/");				
			strcat(temp_path, ep->d_name);

			//ep->type: 4 dir, 8 file
			if ( ep->d_type==4)	// is a directory get into recursively
			{
				if (strcmp(ep->d_name, ".") != 0  && strcmp(ep->d_name, "..")!= 0 )
					recur_del(temp_path,ask);
			}

			else // is a file
			{
				if ( strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..")!= 0 )
				{
					if ( ask==0 && request(temp_path,0) )  // ask to delete 
						erase(temp_path);
					else if (ask==1)	// do not ask - just delete
						erase(temp_path);			 
					//fprintf(stdout,"deleted file %s\n", ep->d_name);
				}
			}
		}
		closedir(dp);

		if ( rmdir(current_path) == -1)
		{
			fprintf(stderr, "Unable to delete directory %s\nDirectory should be empty or there is a permission problem\n",current_path);
			//exit(1);
		}
		//fprintf(stdout,"deleted dir %s\n", current_path);
	}
	free(temp_path);
	return;
}

/*
 * recursive function that enter recursively
 * directories, delete files if they have specified name
 */
void recur_subdir(char *current_path, char *file_name)
{
	DIR *dp;
	struct dirent *ep;
	char *temp_path;
	int source_fd;
	struct stat st;

	temp_path = (char *) malloc( sizeof(char)*MAXPATH+1 );

	dp = opendir(current_path);

	if (dp != NULL) {
		while ( (ep = readdir(dp)) )
		{
			strcpy(temp_path,current_path);

			//if the current dir doesn't finish with '/',
			//I should add it before appending the currente resource name
			if ( current_path[strlen(current_path)-1] != '/')
				strcat(temp_path, "/");				
			strcat(temp_path, ep->d_name);

			//ep->type: 4 dir, 8 file
			if ( ep->d_type==4)	// is a directory get into recursively
			{
				if (strcmp(ep->d_name, ".") != 0  && strcmp(ep->d_name, "..")!= 0 )
					recur_subdir(temp_path,file_name);
			}

			else // is a file
			{
				// check if if file name is the one requested
				if ( strcmp(ep->d_name, file_name) == 0)
				{
					stat(temp_path,&st);
					// do several checks on file than erase
					if ( (source_fd=open (temp_path, O_RDONLY)) == -1)  // try to open file
					{
						fprintf(stderr, "Unable to find file %s or you don\'t have enough permission\n",temp_path);
						continue;
					}

					if (st.st_nlink > 1)  // unlink wouldn't delete for sure the file, whether others open it meanwhile
					{
						fprintf(stderr, "Check that file %s is not open or used by another file\n",temp_path);
						continue;
					}	
					fprintf(stdout,"Found and will be deleted %s\n", temp_path);
					erase(temp_path);
				}
			}
		}
		closedir(dp);
	}
	free(temp_path);
	return;
}

/*
 * del command with various options to delete given files
 */ 
void del(param *list)
{ 	
	param *p, *file_list=NULL;
	char *path,*working_dir, *param_name;
	char *line, *new_line;
	DIR *dir;
	int len=0;
	int source_fd, req_tmp;
	struct stat st;

	// global -> int req=0;
	int read_only=1;
	int hidden=0;
	int ronly=0;
	int attrib=0;
	int sub_dir=0;

	// additional flag to inform that file has been erased yet
	// by other satisfying condition - avoid to try to delete again
	// a deleted file
	int erased=0;

	if(list==NULL)
	{
		fprintf(stderr, "del: missing file operand\n");
		fprintf(stderr, "Try \'help del\' for more information\n");
		exit(1);
	}

	// working directory
	working_dir =(char *) malloc(sizeof(char)*MAXPATH);
	getcwd(working_dir,MAXPATH);

	while (list!=NULL)
	{
		// param name
		param_name= (char *) malloc(sizeof(char)*strlen(list->name)+1);
		strcpy(param_name, list->name);

		if (list->type==0) // parse file
		{
			p=new_elem();

			p->name = (char *) malloc(sizeof(char)*strlen(list->name)+1);
			strcpy(p->name, param_name);
			p->type = 0;
			p->next=NULL;
			insert_e(&file_list,p);
		}
		else // parse option
		{
			// check options
			if ( strcasecmp(param_name,"\\P")==0 ) // req of deletion for each file
				req=1;
			else if ( strcasecmp(param_name,"\\F")==0 ) // force deletion of read-only file
				read_only=0;
			else if ( strcasecmp(param_name,"\\Q")==0 ) // delete everything without asking
			{
				req=0;
				read_only=0;
			}
			else if ( strcasecmp(param_name,"\\S")==0 ) // look for file to be deleted in subdirectories
				sub_dir=1;
			else if ( strncasecmp(param_name,"\\A",2)==0 ) // delete according to attributes
			{
				attrib=1;
				if (strncasecmp(param_name+2,":H",2)==0) // delete only if hidden
					hidden=1;
				else if (strncasecmp(param_name+2,":-H",3)==0) // delete all but hidden
					hidden=-1;
				if (strncasecmp(param_name+2,":R",2)==0) // delete only if readonly
					ronly=1;
				else if (strncasecmp(param_name+2,":-R",3)==0) // delete all but readonly
					ronly=-1;
			}
		}
		list=list->next;
	}

	// parse file list to be deleted
	while(file_list!=NULL)
	{
		erased=0;
		// new file to be delted
		path = (char *) malloc( sizeof(char)*strlen(file_list->name)+1 );
		strcpy(path,file_list->name);

		if (sub_dir!=1) // don't do these checks if it is needed to look in sub dir
		{
			stat(path,&st);
			if (S_ISDIR(st.st_mode)) // skip if directory
			{
				fprintf(stderr, "%s is a directory, try to use DelTree\n",path);
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

		// subdir --> check file in subdir recursively
		if (sub_dir==1)
		{
			line = strrchr(path,'/');

			// length of string not composing file name
			if (line==NULL)
				len = strlen(working_dir);
			else
				len = strlen(path)-strlen(line);

			new_line = (char*)malloc(sizeof(char)*len+1);

			if (line==NULL) // only file name no dir specified
				strcpy(new_line, working_dir);
			else
			{
				strncpy(new_line, path, len);
				line++;
			}
			dir=opendir(new_line);
			if (dir == NULL)
			{
				fprintf(stderr, "Unable to find file %s or you don\'t have enough permission\n",path);
				exit(1);
			}

			if (line==NULL) // looking in current dir
				recur_subdir(new_line,path);
			else	 // looking in sub_dir
				recur_subdir(new_line,line);

			erased=1;
		}

		// delete according to given attribute
		if(attrib==1)
		{

			if (ronly==1) // only readdonly
			{
				if ( rd_only(path) && erased!=1 )
				{
					erase(path);
					close(source_fd);
				}
			}
			else if (ronly==-1) // everything but readonly
			{
				if ( !rd_only(path) && erased!=1 )
				{
					erase(path);
					close(source_fd);
				}
			}
			if (hidden==1 && erased!=1) // only hidden
			{
				if ( strncmp(path,".",1)==0 )
				{
					//fprintf(stdout,"here hidden\n");
					erase(path);
					close(source_fd);
				}
			}
			else if (hidden==-1)// everything but hidden
			{
				if ( strncmp(path,".",1)!=0 && erased!=1)
				{
					erase(path);
					close(source_fd);
				}
			}
			erased=1;
		}

		// ask only for read-only files
		if (read_only==1 && erased!=1) 
		{

			if ( rd_only(path) )
			{ 
				req_tmp=req; // inhibit req value
				req=0;
				if ( request(path,1) ) // ask for request in case
				{
					erase(path);
					erased=1;
					close(source_fd);
				}
				req=req_tmp;
			}

			else // just delete
			{
				erase(path);
				erased=1;
				close(source_fd);
			}

		}

		// just delete		
		else if (erased!=1)
		{
			erase(path);
			erased=1;
			close(source_fd);
		}
		free(path);
		file_list=file_list->next;
	}
	free(working_dir);
}

/* 
 * deltree command --> rm -Rf
 */
void deltree(param *list)
{
	char *working_dir;
	struct stat st;
	int source_fd;
	req=0;

	if(list==NULL)
	{
		fprintf(stderr, "deltree: missing file operand\n");
		fprintf(stderr, "Try \'help deltree\' for more information\n");
		exit(1);
	}

	// working directory
	working_dir =(char *) malloc(sizeof(char)*MAXPATH);
	getcwd(working_dir,MAXPATH);

	while (list!=NULL)
	{
		if (list->type==1) // no option for this command
		{
			fprintf(stderr, "%s - unknown option\n",list->name );
			exit(1);
		}
		else // files
		{
			if (stat(list->name,&st)==-1)
			{
				fprintf(stderr, "Failed to remove \'%s\': No such file or directory\n",list->name);
				exit(1);
			}

			// if file delete but warn
			if (S_ISREG(st.st_mode))
			{
				if ( (source_fd=open (list->name, O_RDONLY)) == -1)  // try to open file
				{
					fprintf(stderr, "Unable to find file %s or you don\'t have enough permission\n",list->name);
					exit(1);
				}

				if (st.st_nlink > 1)  // unlink wouldn't delete for sure the file, whether others open it meanwhile
				{
					fprintf(stderr, "Check that file %s is not open or used by another file\n",list->name);
					exit(1);
				}	
				erase(list->name);
				fprintf(stdout,"Warning: file %s deleted, but you can use\n\'del\' command to delete single files\n",list->name);
				close(source_fd);
			} 

			// delete recursively if directory
			else if(S_ISDIR(st.st_mode))
			{
				if (opendir(list->name)==NULL)
				{
					fprintf(stderr, "Unable to open directory %s\n", list->name);
					exit(1);
				}
				// rm -Rf
				recur_del(list->name,1);
			}
		}

		list=list->next;
	}
}	

/*
 * rmdir
 */
void rd(param *list)
{
	param *p, *file_list=NULL;
	char *working_dir;
	struct stat st;
	int sub_dir=0, no_ask=0;
	req=0;

	if(list==NULL)
	{
		fprintf(stderr, "rd: missing file operand\n");
		fprintf(stderr, "Try \'help rd\' for more information\n");
		exit(1);
	}

	// working directory
	working_dir =(char *) malloc(sizeof(char)*MAXPATH);
	getcwd(working_dir,MAXPATH);

	while (list!=NULL)
	{
		if (list->type==1) // options
		{
			// check options
			if ( strcasecmp(list->name,"\\S")==0 ) // delete recursively also subdirectories
				sub_dir=1;
			else if ( strcasecmp(list->name,"\\Q")==0 ) // delete everything without asking
				no_ask=1;
			else
			{
				fprintf(stderr, "%s - unknown option\n",list->name );
				exit(1);
			}
		}
		else // dir name
		{
			p=new_elem();

			p->name = (char *) malloc(sizeof(char)*strlen(list->name)+1);
			strcpy(p->name, list->name);
			p->type = 0;
			p->next=NULL;
			insert_e(&file_list,p);
		}

		list=list->next;
	}

	// parse dir list to be deleted
	while(file_list!=NULL)
	{	
		if (stat(file_list->name,&st)==-1)
		{
			fprintf(stderr, "Failed to remove \'%s\': No such file or directory\n",file_list->name);
			exit(1);
		}

		// delete directory
		if(S_ISDIR(st.st_mode))
		{
			if (opendir(file_list->name)==NULL)
			{
				fprintf(stderr, "Unable to open directory %s\n", file_list->name);
				exit(1);
			}

			if (sub_dir==1)
				// rm -Rf
				recur_del(file_list->name,no_ask);
			else // only delete dir if empty
			{
				if(empty_dir(file_list->name))  // dir is empty
				{
					if ( rmdir(file_list->name) == -1)
					{
						fprintf(stderr, "Unable to delete directory %s\nDirectory should be empty or there is a permission problem\n",file_list->name);
						exit(1);
					}
				}

				else // dir not empty
				{
					fprintf(stderr, "Unable to delete directory %s\nDirectory should be empty or there is a permission problem\n",file_list->name);
					exit(1);
				}
			}
		}
		file_list=file_list->next;
	}
	free(working_dir);
}
