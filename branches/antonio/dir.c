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

//TODO recursion algorithm
//TODO printout formatting functions
//TODO getline realloc: maybe unsigned int length = u + strlen(str); without strlen
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>  
#include <dirent.h>
#include <semaphore.h>
#include "resource.h"
//TODO better definition of maximum path length

//Looking in /usr/src/linux-2.4.20-8/include/linux/limits.h, I see:
//#define PATH_MAX 4096 /* # chars in a path name including nul */
//TODO define globally for all files
#define MAXPATH 4096
/* 
 * D  Directory, R  File sola lettura, H  File nascosti
 * A  File archivio, S  File di sistema, - Prefisso per negare l'attributo
 * 
 */
//deafult views /A option

short show_directory;
short show_read_only;
short show_hidden_files;
short show_system_files;
short show_archive_files;

short show_owner; // /Q option 
short show_recursive; // /S option 

Resource *resources_list;

//it initialize tha mask with default options
void initialize() {

	show_directory = 1;
	show_read_only = 0;
	show_hidden_files = 0;
	show_system_files = 0; //TODO do "system" files exist under linux ?
	show_archive_files = 1;//TODO do archive files exist under linux ?

	show_owner = 0; // /Q option 
	show_recursive = 1; // /S option //TODO to change in 0
	resources_list = NULL;

}

Resource *create_res(struct stat status, char res_name[], unsigned char type) {

	Resource *res;
	char *name;

	name =(char *) malloc((unsigned int)strlen(res_name));
	strcpy(name, res_name);

	res = new_resource();
	res->status = status;
	res->name = name;
	res->type = type;
	return res;

}

Resource *my_dir(char *path) {

	DIR *dp;
	Resource *res;
	Resource *to_print, *temp;

	struct dirent *ep;
	struct stat status;
	char current_dir[MAXPATH], temp_path[MAXPATH];
	char *res_name;
	int *p;

	strcpy(current_dir, path);

	dp = opendir(current_dir);
	if (dp != NULL) {
		while (ep = readdir(dp)) {

			//ep->type: 4 dir, 8 file
			if (show_directory==0 && ep->d_type==4) {
				continue;
			}

			//if ep = . or .. , i don't need to process the node
			
			if ( (strcmp(ep->d_name, ".") == 0 ) || (strcmp(ep->d_name, "..")
					== 0)) {
				//printf("%s\n",ep->d_name);
				continue;
			}
			if (show_hidden_files==0 && ep->d_name[0]=='.') {
				continue;
			}

			//at every round, i reset the temp_path to current dir value
			strcpy(temp_path, current_dir);

			//if the current dir doesn't finish with '/',i should add it before appending the currente resource name
			if ( ((char)current_dir[strlen(current_dir)-1]) != '/') {

				strcat(temp_path, "/");

			}
			strcat(temp_path, ep->d_name);

			//TODO recursion
			/*
			 if(show_recursive==1 && ep->d_type==4){
			 my_dir(temp_path);
			 }*/

			//TODO files that terminates with "~" aren't added to the list
			if (temp_path[strlen(temp_path)-1] == '~') {
				//printf("%s\n",ep->d_name);
				continue;
			}

			if ( (p=open(temp_path, O_EXCL)) == NULL) {

				printf("dir: cannot access : %s: No such file or directory\n",
						temp_path);
				continue;
			}

			if (fstat(p, &status) != 0) {
				if (ep->d_type==4)
					printf("Cannot open directory %s: Permission denied\n",
							temp_path);
				else
					printf("Cannot open file %s: Permission denied\n",
							temp_path);
				continue;

			}

			//st.mode 400 (user read only)-> 33024
			//TODO should i menage other read only cases ?
			if (show_read_only==1
					&& (((unsigned short)status.st_mode) == 33024))
				continue;

			res = create_res(status, ep->d_name, ep->d_type);
			insert_resource(&resources_list, res);

		}
		(void) closedir(dp);
		/*
		while (resources_list!= NULL) {
			insert_resource(&to_print, get_resource(&resources_list));
		}
		print_list(to_print, current_dir);
		*/
	} else{
		printf("dir: cannot access : %s: No such file or directory\n", path);
		//return to_print;//TODO correct
	}	
	return resources_list;
}

int main(int argc, char **argv) {

	Resource *to_print;
	Resource *to_dir;
	 
	char current_dir[MAXPATH];

	if (argc != 2) {
		printf("Please specify one argument\n");
		return -1;
	}

	//TODO initialize with correct parameters/options
	initialize();

	strcpy(current_dir, argv[1]);
	
	
	to_print = my_dir(current_dir);
	print_list(to_print, current_dir);
	
	
	
	return 0;
}
