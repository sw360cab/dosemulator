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
 *+
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and  limitations under the
 * License.
 *
 * ***** END LICENSE BLOCK ***** 
 */

//status.st_mode = 32768 + xxx xxx xxx (converted in decimal)
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "parse.h"
#include "resource.h"

/*returns the full path of the folder containing the file,
 * whose path is passed as argument*/
char *extract_path(char *);

/*prints permissions (windows like format and meaning) of the resource and its 
 * name (with full path) */
void print_mask(int[], char *, char *);

/* build the permission mask of the resource*/
void print_attrib(Resource *, char *);

/* print on the screen the failure of file access and exit*/
void attrib_error();

/* collect resources from the directory (if argument is a dir) in a list (if argument was a file , 
 * only one element in the list). Check for recursion and filters arguments. Change permission bits of resource*/
Resource *my_attrib(char *);

/* element of the recursion alghoritm, it launhes my_attrib function and return its output*/
Resource *attrib_processNode(char *);

/* starting from a path, it process the node and recursively all its descent */
Resource *attrib_followNode(char *);

/* scan for parameters and launch the executive functions of attrib command*/
void attrib(param *);

extern Resource *create_res(struct stat, char[], unsigned char, char *);
extern int is_read_only(long);
extern void dec2bin(long, char*);

//options: 0 deafult, 1=+=put attribute,2=-=remove attribute
short int r_option=0;
short int w_option=0;
short int x_option=0;
short int h_option=0;
Resource *res_list= NULL;

//S and D to use without previous arguments
short int s_parameter=0; //S is recursion only if used without arguments 
short int d_option=0; //d valid only if s_parameter is 1;


struct stat st;

/*returns the full path of the folder containing the file,
 whose path is passed as argument*/
char *extract_path(char *fullname) {
	int i=0, position=0;
	char *new_string;
	char *temp =(char *)malloc(2);
	short flag= FALSE;

	for (i=strlen(fullname)-1; i>-1; i--) {
		if (fullname[i]=='/') {
			position =i;
			i=-1;
			flag=TRUE;
		}

	}
	if (flag==TRUE) {
		new_string = (char *)malloc(position);

		for (i=0; i<position; i++) {

			sprintf(temp, "%c", fullname[i]);

			if (i==0)
				strcpy(new_string, temp);
			else
				strcat(new_string, temp);

		}
	} else {

		new_string = (char *)malloc(MAXPATH);
		getcwd(new_string, BUF_MAX);

	}

	return new_string;

}

/*prints permissions (windows like format and meaning) of the resource and its 
 *  name (with full path) */
void print_mask(int mask[], char *path, char *name) {

	char *full_path = (char *) malloc(strlen(path)+ strlen(name) +2);
	char attributes[4];

	strcpy(full_path, path);

	if ( ((char)full_path[strlen(path)-1]) != '/') {

		strcat(full_path, "/");
	}
	strcat(full_path, name);

	if (mask[0] == 1)
		attributes[0]='R';
	else
		attributes[0]=' ';

	if (mask[1] == 1)
		attributes[1]='W';
	else
		attributes[1]=' ';

	if (mask[2] == 1)
		attributes[2]='X';
	else
		attributes[2]=' ';

	if (mask[3] == 1)
		attributes[3]='H';
	else
		attributes[3]=' ';

	fprintf(stdout,"%c%c%c%c   %s\n", attributes[0], attributes[1], attributes[2],
	attributes[3], full_path);

	//free(full_path);
}

/*build the permission mask of the resource */
void print_attrib(Resource *res, char *path) {

	Resource *first;
	mode_t mode;
	char binary[80];
	int mask[4];//R,W,X,H
	mask[0] = 0;
	mask[1] = 0;
	mask[2] = 0;
	mask[3] = 0;

	if (res == NULL)
		return;

	first = res->next;
	res = res->next;

	//TODO fill mask
	while (res->next != first) {

		mask[0] = 0;
		mask[1] = 0;
		mask[2] = 0;
		mask[3] = 0;

		//mask = fill_mask(status.st_mode, ep->d_name);


		dec2bin((long)res->status.st_mode, (char *) &binary);
		//8 9 10 - 11 12 13 - 14 15 16

		if (is_read_only((long)res->status.st_mode)==1)
			mask[0]=1;

		//at least one write enabled
		//if (binary[9]=='1' || binary[12]=='1' || binary[15]=='1')
		//	mask[1]=1;
		mode = res->status.st_mode & S_IWRITE;
		if (mode == S_IWRITE)
			mask[1]=1;

		//at least one execute enable
		//if (binary[10]=='1' || binary[13]=='1' || binary[16]=='1')

		mode = res->status.st_mode;
		mode = res->status.st_mode & S_IEXEC;
		if (mode == S_IEXEC)
			mask[2]=1;

		if (res->name[0]=='.') {

			mask[3]=1;

		}

		print_mask(mask, path, res->name);

		res=res->next;

	}

	mask[0] = 0;
	mask[1] = 0;
	mask[2] = 0;
	mask[3] = 0;

	if (is_read_only((long)res->status.st_mode)==1)
		mask[0]=1;

	dec2bin((long)res->status.st_mode, (char *) &binary);
	//8 9 10 - 11 12 13 - 14 15 16

	//at least one write enabled
	//if (binary[9]=='1' || binary[12]=='1' || binary[15]=='1')
	mode = res->status.st_mode;
	mode = res->status.st_mode & S_IWRITE;
	if (mode == S_IWRITE)
		mask[1]=1;

	//at least one execute enable
	//if (binary[10]=='1' || binary[13]=='1' || binary[16]=='1')
	mode = res->status.st_mode;
	mode = res->status.st_mode & S_IEXEC;
	if (mode == S_IEXEC)
		mask[2]=1;

	if (res->name[0]=='.')
		mask[3]=1;

	print_mask(mask, path, res->name);

}
/* print on the screen the failure of file access and exit*/
void attrib_error() {

	fprintf(stdout,"ATTRIB: permission denied\n");
	exit(1);

}

/* collect resources from the directory (if argument is a dir) in a list (if argument was a file , 
 * only one element in the list). Check for recursion and filters arguments. Change permission bits of resource*/
Resource *my_attrib(char *path) {

	DIR *dp;

	Resource *temp, *to_return=NULL, *to_print=NULL, *temp2;
	struct dirent *ep;
	int p;
	char *temp_path = (char *)malloc((unsigned int)MAXPATH);
	char *name = (char *)malloc((unsigned int)MAXPATH);
	mode_t mode;
	struct stat status;
	char binary[80];
	char *extracted_path=(char *)malloc(MAXPATH);

	strcpy(temp_path, path);

	//if no parameters, print info
	if (r_option + w_option + h_option + x_option == 0) {

		if (stat(path, &status) == -1) {
			fprintf(stderr,"ATTRIB: can't access %s\n",path);
			exit(1);
		}
		if (S_ISDIR(status.st_mode)) {

			dp = opendir(path);

			//scan for files
			if (dp != NULL) {
				while ( (ep=readdir(dp) )) {
					strcpy(temp_path, path);
					if (ep->d_name[strlen(ep->d_name)-1] == '~')
						continue;

					strcpy(temp_path, (char *)build_path(path, ep->d_name));

					strcpy(name, ep->d_name);

					if (strcmp(name, ".")==0 || strcmp(name, "..")==0)
						continue;

					if ( (p= open(temp_path, O_RDONLY)) == -1) {

						fprintf(stderr,"ATTRIB: 1cannot access : %s: No such file or directory\n",
						temp_path);
						//no need to exit; continue;
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

					//if hidden, yes
					if (name[0] == '.') {

						temp2
								= create_res(status, ep->d_name, ep->d_type,
										path);
						insert_resource(&to_print, temp2);

					}

					if (d_option==FALSE) {//no folders
						if (! (ep->d_name[0]=='.' || strcmp(name, "..")==0
								|| ep->d_name[strlen(ep->d_name)-1] == '~' || ep->d_type==4)) {
							temp2 = create_res(status, ep->d_name, ep->d_type,
									path);
							insert_resource(&to_print, temp2);
						}
					} else {//yes folders

						if (! (ep->d_name[0]=='.' || strcmp(name, "..")==0
								|| ep->d_name[strlen(ep->d_name)-1] == '~')) {
							temp2 = create_res(status, ep->d_name, ep->d_type,
									path);
							insert_resource(&to_print, temp2);
						}

					}
					//if folder
					//attrib /home/folletto/temp \S
					if (ep->d_type == 4) {

						if (s_parameter==0) {
							close(p);
							continue;
						} else {//s_paramter != 0, return list of directories


							/*if (ep->d_name[0]=='.' || ep->d_name[0]=='..' || ep->d_name
							 =='~')
							 continue;*/
							if (strcmp(ep->d_name, "..")==0 || strcmp(
									ep->d_name, "..")==0 || ep->d_name[strlen(ep->d_name)-1] == '~')
								continue;

							else {
								temp = create_res(status, ep->d_name,
										ep->d_type, path);
								insert_resource(&to_return, temp);
								//fprintf(stdout,"resource %s inserted, if folder type 4, %d\n",temp->name, temp->type);
							}

							if (d_option==0)//if d option, don't process folder
								continue;

						}

					}
					close(p);
				}

				print_attrib(to_print, path);
				closedir(dp);
			}
		} else {//if file


			stat(path, &status);

			strcpy(extracted_path, extract_path(temp_path));

			temp = create_res(status, path, (unsigned char)8,
					extract_path(temp_path));
			insert_resource(&to_print, temp);

			print_attrib(to_print, extracted_path);
		}

	} else {

		if ( (p = open(path, O_RDONLY)) == -1) {

			fprintf(stderr,"ATTRIB: 2cannot access : %s: No such file or directory, or permission denied\n", path);
			return NULL;
		}

		if (stat(path, &status) != 0) {
			if (ep->d_type==4)
				fprintf(stdout,"Cannot open resource %s: Permission denied\n", path);

			return NULL;

		}

		dec2bin((long)status.st_mode, (char *)&binary);
		//8 9 10 - 11 12 13 - 14 15 16

		//chmod +r d.txt
		//TODO! try to do it with set mode 


		if (r_option == 1) {

			mode = status.st_mode | S_IRUSR | S_IRGRP | S_IROTH;

			if (chmod(path, mode)==-1) {
				attrib_error();
			}

		}

		if (r_option == 2) {

			mode = status.st_mode & !( S_IRUSR | S_IRGRP | S_IROTH);

			if (chmod(path, mode)==-1) {
				attrib_error();
			}

		}
		if (w_option == 1) {

			mode = status.st_mode | S_IWUSR | S_IWGRP | S_IWOTH;

			if (chmod(path, mode) == -1) {
				attrib_error();
			}

		}
		if (w_option == 2) {

			mode = status.st_mode & !( S_IWUSR | S_IWGRP | S_IWOTH);

			if (chmod(path, mode) == -1) {
				attrib_error();
			}
		}
		if (x_option == 1) {

			mode = status.st_mode | S_IXUSR | S_IXGRP | S_IXOTH;

			if (chmod(path, mode) == -1) {
				attrib_error();
			}

		}
		if (x_option == 2) {
			mode = status.st_mode & !(S_IXUSR | S_IXGRP | S_IXOTH);

			if (chmod(path, mode) == -1) {
				attrib_error();
			}

		}
		close(p);
	}

	/*
	 free(command);
	 free(temp_path);
	 free(name);*/

	return to_return;
}

/* element of the recursion alghoritm, it launhes my_attrib function and return its output*/
Resource *attrib_processNode(char *path) {

	Resource *to_print=NULL;

	to_print = my_attrib(path);

	return to_print;

}

/* starting from a path, it process the node and recursively all its descent */
Resource *attrib_followNode(char *path) {

	Resource *children=NULL, *first;
	children = (Resource *) malloc(sizeof(Resource));

	children = attrib_processNode(path);

	if (children != NULL) {

		first=children->next;
		children=children->next;

		while (children->next!=first) {

			attrib_followNode((char *)build_path(children->path, children->name));

			children=children->next;
		}

		attrib_followNode((char *)build_path(children->path, children->name));

	}

	//TODO no return, but it's return isn't void

	//free(children);
	return children;
}

/* scan for parameters and launch the executive functions of attrib command*/
void attrib(param *parameters) {

	param *p = parameters;
	char c=0;
	char *current_dir;

	//first: search for option
	while (p!= NULL) {

		if (p->type==1) {

			if (strcasecmp(p->name, "+R") == 0)
				r_option=1;
			else if (strcasecmp(p->name, "-R") == 0)
				r_option=2;
			else if (strcasecmp(p->name, "+H") == 0)
				h_option=1;
			else if (strcasecmp(p->name, "-H") == 0)
				h_option=2;
			else if (strcasecmp(p->name, "+W") == 0)
				w_option=1;
			else if (strcasecmp(p->name, "-W") == 0)
				w_option=2;
			else if (strcasecmp(p->name, "+X") == 0)
				x_option=1;
			else if (strcasecmp(p->name, "-X") == 0)
				x_option=2;
			else if (strcasecmp(p->name, "\\S") == 0)
				s_parameter=1;
			else if (strcasecmp(p->name, "\\D") == 0)
				d_option=1;
			else {
				fprintf(stdout,"ATTRIB: not valid argument\n");
				exit(1);
			}
		}

		p=p->next;
	}

	if (d_option==TRUE && s_parameter==FALSE) {
		fprintf(stderr,"Option \\D is valid only if combinated with option \\S \n");
		exit(1);
	}

	p = parameters;

	//second: search for paths
	while (p!= NULL) {

		if (p->type==0) {
			c++;

			if (s_parameter==FALSE)

				attrib_processNode(p->name);

			else

				attrib_followNode(p->name);

		}

		p=p->next;
	}

	//if no paths
	if (c==0) {

		current_dir = (char *) malloc((unsigned int)MAXPATH);
		getcwd(current_dir, MAXPATH);

		if (s_parameter==FALSE)

			attrib_processNode(current_dir);

		else

			attrib_followNode(current_dir);

	}

	//free(current_dir);

}

