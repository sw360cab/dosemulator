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

//TODO printout formatting functions -> most common functions: see my_commands.doc  
//TODO !fare il caso di più path
//TODO readonly dir ?
//TODO close fd in all my functions!!
//TODO in all files: substitue par = (*parameter_list) with par = parameter_list;
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

//TODO try to move create_res to resource.c
Resource *create_res(struct stat status, char res_name[], unsigned char type,
		char *path) {

	Resource *res;
	char *name, *new_path;

	name =(char *) malloc((unsigned int)strlen(res_name));
	new_path =(char *)malloc((unsigned int)MAXPATH);

	strcpy(name, res_name);
	strcpy(new_path, path);

	res = new_resource();
	res->status = status;
	res->name = name;
	res->type = type;
	res->path = new_path;

	return res;

}

Resource *my_dir(char *path) {

	DIR *dp;
	Resource *res;
	Resource *to_print= NULL;

	struct dirent *ep;
	struct stat status;
	struct statvfs statusvfs;
	char current_dir[MAXPATH], temp_path[MAXPATH];
	int *p;

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
			if ( (show_hidden_files==2  )
					&& ep->d_name[0]=='.')
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

			if ( (p=(int *)open(temp_path, O_EXCL)) == NULL) {

				fprintf(stderr,"DIR: cannot access : %s: No such file or directory\n",
				temp_path);
				exit(1);
			}

			if (fstat((int)p, &status) != 0) {
				if (ep->d_type==4)
					printf("Cannot open directory %s: Permission denied\n",
							temp_path);
				else
					printf("Cannot open file %s: Permission denied\n",
							temp_path);
				continue;

			}

			//if stat succeeded, statvfs will succeed, no errors check

			//st.mode 400 (user read only)-> 33024
			//TODO other read only cases: see attrib.c
			if (show_read_only==1
					&& (((unsigned short)status.st_mode) != 33024))
				continue;
			if (show_read_only==2
					&& (((unsigned short)status.st_mode) == 33024))
				continue;

			res = create_res(status, ep->d_name, ep->d_type, path);
			insert_resource(&resources_list, res);

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

Resource *processNode(char *path) {

	Resource *to_print=NULL, *to_dir=NULL;

	to_print = my_dir(path);
	to_dir = print_list(to_print, path, (char *)parameters_global);

	return to_dir;
}

char *build_path(char *parent_path, char* resource_name) {

	char *new_path = malloc((unsigned int) MAXPATH);
	strcpy(new_path, parent_path);
	strcat(new_path, "/");
	strcat(new_path, resource_name);

	return new_path;

}

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

//int main(int argc, char **argv) {

void dir(param **parameters) {

	char *current_dir;
	param *temp = (*parameters);
	short int flag=FALSE;
	
	//here i initialize with default values 
	initialize();
	parameters_global = (*parameters);
	
	while (temp != NULL) {
		
		//printf("%s %d\n",temp->name,temp->type);
		
		
		if (temp->type == 0) {

			current_dir = (char *)malloc((unsigned int)strlen(temp->name));
			strcpy(current_dir, temp->name);
			flag = TRUE;
		}
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
		
		else {//TODO iniatialize with correct parameters/options -> all cases
			
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
			else if(strcasecmp(temp->name, "\\S") == 0)
				show_recursive = 1;

			else {
				printf("DIR : not valid argument\n");
				exit(1);
			}

		}
		temp = temp->next;
	}
	
	
	temp = (*parameters);
	
	if(flag==FALSE || temp==NULL ){
		
	
		current_dir = (char *)malloc((unsigned int)MAXPATH);
		getcwd(current_dir,MAXPATH);
	}
	
	
	if (show_recursive == 0) {

		processNode(current_dir);
	} else
		followNode(current_dir);

	free(current_dir);
	return;
}