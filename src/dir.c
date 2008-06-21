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

//BUG dir /home/folletto/temp : sometimes strange chars appear before size

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>  
#include <sys/statvfs.h>
#include <dirent.h>
#include <semaphore.h>
#include "resource.h"
#include "parse.h"

//it initialize the mask with default options
void initialize() ;


/* it fills and return the list of all resources fuonded in the path
(with respect to filter arguements */
Resource *my_dir(char *);

/* launch my_dir capturing the return  and launch the printing function*/
Resource *processNode(char *) ;

/* process the path passed as argument, then for all children resource, it launches itself for recursion  */
void followNode(char *);

/* check for and set parameters, launch dir execution functions */
void dir(param *);

/* 
 * D  Directory, R  File sola lettura, H  File nascosti
 * A  File archivio, S  File di sistema, - Prefisso per negare l'attributo
 * 
 */
//deafult views /A option

short show_directory;
short show_read_only;
short show_hidden_files;
short show_owner; // /Q option 
short show_recursive; // /S option 

Resource *resources_list;
param *parameters_global;

extern Resource *create_res(struct stat,char[],unsigned char,char*);
extern char *build_path(char*,char*);
extern int is_read_only(long);

//it initialize tha mask with default options
void initialize() {

	//0 default, 1 show only , 2 don't show 
	show_directory = 0;
	show_read_only = 0;
	show_hidden_files = 2;
	show_owner = 0; // /Q option 
	show_recursive = 0;
	resources_list = NULL;
	parameters_global = NULL;
}


/* it fills and return the list of all resources fuonded in the path(with respect to filter arguements */
Resource *my_dir(char *path) {

	DIR *dp;
	Resource *res;
	Resource *to_print= NULL;

	struct dirent *ep;
	struct stat status;
	char current_dir[MAXPATH], temp_path[MAXPATH];
	int p;

	strcpy(current_dir, path);

	dp = opendir(current_dir);
	if (dp != NULL) {
		while ( (ep = readdir(dp))) {

			//CASE DIR ep->type: 4 dir, 8 file
			if ( (show_hidden_files==1 || show_read_only==1 || show_directory
					==2) && ep->d_type==4)
				continue;

			//CASE FILE: if show only dir, continue
			if (show_directory==1 && ep->d_type!=4)
				continue;

			//if ep = . or .. , i don't need to process the node
			if ( (strcmp(ep->d_name, ".") == 0 ) || (strcmp(ep->d_name, "..")
					== 0)) {
				continue;
			}

			//if hidden files exclude
			if ( (show_hidden_files==2 ) && ep->d_name[0]=='.')
				continue;

			//if show only hidden files, don't show normal files 
			if (show_hidden_files==1 && ep->d_name[0]!='.')
				continue;
			//at every round, i reset the temp_path to current dir value
			strcpy(temp_path, current_dir);

			//if the current dir doesn't finish with '/',i should add it before appending the currente resource name
			if ( ((char)current_dir[strlen(current_dir)-1]) != '/') {

				strcat(temp_path, "/");

			}
			strcat(temp_path, ep->d_name);

			//files that terminates with "~" aren't added to the list
			if (temp_path[strlen(temp_path)-1] == '~') {
				continue;
			}

			
			if ( (p= open(temp_path, O_RDONLY)) == -1) {

				fprintf(stderr,"DIR: cannot access : %s: No such file or directory\n",
				temp_path);
				//exit(1);no need to exit
			}

			if (stat(temp_path, &status) != 0) {
				if (ep->d_type==4)
					fprintf(stdout,"Cannot open directory %s: Permission denied\n",
							temp_path);
				else
					fprintf(stdout,"Cannot open file %s: Permission denied\n",
							temp_path);
				continue;

			}

			//if stat succeeded, statvfs will succeed, no errors check

			//st.mode 400 (user read only)-> 33024
			
			
			if (show_read_only==1
					&& (is_read_only(status.st_mode)==0))
				continue;
			if (show_read_only==2
					&& (is_read_only(status.st_mode)==1))
				continue;

			res = create_res(status, ep->d_name, ep->d_type, path);
			insert_resource(&resources_list, res);
			
			close(p);
		}
		(void) closedir(dp);

		while (resources_list!= NULL) {
			insert_resource(&to_print, get_resource(&resources_list));
		}
		resources_list = NULL;
		
		
	} else {
		fprintf(stderr,"DIR: cannot access : %s: No such file or directory\n", path);
		exit(1);

	}
	return to_print;
}

/* launch my_dir capturing the return  and launch the printing function*/
Resource *processNode(char *path) {

	Resource *to_print=NULL, *to_dir=NULL;

	to_print = my_dir(path);
	to_dir = print_list(to_print, path, (char *)parameters_global);

	return to_dir;
}


/* process the path passed as argument, then for all children resource, it launches itself for recursion  */
void followNode(char *path) {

	Resource *children=NULL, *first;

	children = processNode(path);

	if (children != NULL) {

		first=children->next;
		children=children->next;

		while (children->next!=first) {

			followNode(build_path(children->path, children->name));

			children=children->next;
		}

		followNode(build_path(children->path, children->name));
	}
}


/* check for and set parameters, launch dir execution functions */
void dir(param *parameters) {

	char *current_dir;
	param *temp = parameters;
	short int flag=FALSE;
	short int count=0;
	//here i initialize with default values 
	initialize();
	parameters_global = parameters;

	//first list check: initialize parameters
	while (temp != NULL) {

		/*
		 * D  Directory, R  File sola lettura, H  File nascosti
		 * A  File archivio, S  File di sistema, - Prefisso per negare l'attributo
		 * default:
		 * show_directory = 0;
		 show_read_only = 0;
		 show_hidden_files = 0;
		 show_owner = 0; // /Q option 
		 show_recursive = 0;
		 
		 0=default
		 1=show only
		 2=exclude
		 */
		if (temp->type == 1) {

			if (strcasecmp(temp->name, "\\AD") == 0)
				show_directory=1;
			else if (strcasecmp(temp->name, "\\A-D") == 0)
				show_directory=2;
			else if (strcasecmp(temp->name, "\\AR") == 0)
				show_read_only=1;
			else if (strcasecmp(temp->name, "\\A-R") == 0)
				show_read_only=2;
			else if (strcasecmp(temp->name, "\\AH")== 0)
				show_hidden_files=1;
			else if (strcasecmp(temp->name, "\\A-H")== 0)
				show_hidden_files=2;
			else if (strcasecmp(temp->name, "\\A")== 0) //all
				show_hidden_files=0;
			else if (strcasecmp(temp->name, "\\S") == 0)
				show_recursive = 1;

			else {
				fprintf(stdout,"DIR : not valid argument\n");
				exit(1);
			}

		}
		temp = temp->next;
	}

	temp = parameters;

	//second list check: look for paths
	while (temp != NULL) {
		flag = FALSE;
		count++;
		
		if (temp->type == 0) {

			current_dir = (char *)malloc((unsigned int)strlen(temp->name));
			strcpy(current_dir, temp->name);
			flag = TRUE;
			
		}

		if (flag==FALSE) {

			current_dir = (char *)malloc((unsigned int)MAXPATH);
			getcwd(current_dir, MAXPATH);
		}

		if (show_recursive == 0) {

			processNode(current_dir);
		} else
			followNode(current_dir);


		temp = temp->next;
	}
	
	
	
	if ( flag==FALSE && count==0) {

		current_dir = (char *)malloc((unsigned int)MAXPATH);
		getcwd(current_dir, MAXPATH);

		if (show_recursive == 0) {

			processNode(current_dir);
		} else
			followNode(current_dir);

	}

	//free(current_dir);
	return;
}
