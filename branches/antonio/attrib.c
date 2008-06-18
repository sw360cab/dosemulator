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
#include "parse.h"
#include "resource.h"

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

	printf("%c%c%c%c   %s\n", attributes[0], attributes[1], attributes[2],
			attributes[3], full_path);

	//free(full_path);
}
/*
 int *fill_mask(mode_t st_mode, char *d_name) {

 int mask[4];
 char binary[80];
 //read only = 100 000 000 || 100 100 000 || 100 100 100
 if (((st_mode) == 33024) || st_mode == 33056 || st_mode == 33060)
 mask[0]=1;

 dec2bin((long)st_mode,(char *) &binary);
 //8 9 10 - 11 12 13 - 14 15 16

 //at least one write enabled
 if (binary[9]=='1' || binary[12]=='1' || binary[15]=='1')
 mask[1]=1;

 //at least one execute enable
 if (binary[10]=='1' || binary[13]=='1' || binary[16]=='1')
 mask[2]=1;

 if (d_name[0]=='.')

 mask[3]=1;

 return mask;

 }*/

void print_attrib(Resource *res, char *path) {

	Resource *first;
	char binary[80];
	int mask[4];//R,W,X,H


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
		if (binary[9]=='1' || binary[12]=='1' || binary[15]=='1')
			mask[1]=1;

		//at least one execute enable
		if (binary[10]=='1' || binary[13]=='1' || binary[16]=='1')
			mask[2]=1;

		if (res->name[0]=='.') {

			mask[3]=1;

		}

		print_mask(mask, path, res->name);

		res=res->next;

	}

	if (res->status.st_mode == 33024 || res->status.st_mode == 33056
			|| res->status.st_mode == 33060)
		mask[0]=1;

	dec2bin((long)res->status.st_mode, (char *) &binary);
	//8 9 10 - 11 12 13 - 14 15 16

	//at least one write enabled
	if (binary[9]=='1' || binary[12]=='1' || binary[15]=='1')
		mask[1]=1;

	//at least one execute enable
	if (binary[10]=='1' || binary[13]=='1' || binary[16]=='1')
		mask[2]=1;

	if (res->name[0]=='.')
		mask[3]=1;

	print_mask(mask, path, res->name);

}
/*
 long bin2dec(char *s){
 long r=0;
 for (; *s; r = (r<<1) | (*s++ - '0'));
 return r;
 } */

int bin2dec(char *bin) {

	int b, k, m, n;

	int len, sum = 0;

	len = strlen(bin) - 1;

	for (k = 0; k <= len; k++)

	{

		n = (bin[k] - '0'); // char to numeric value

		if ((n > 1) || (n < 0))

		{

			puts("\n\n ERROR! BINARY has only 1 and 0!\n");

			return (0);

		}

		for (b = 1, m = len; m > k; m--)

		{

			// 1 2 4 8 16 32 64 ... place-values, reversed here

			b *= 2;

		}

		// sum it up

		sum = sum + n * b;

		//printf("%d*%d + ",n,b); // uncomment to show the way this works

	}

	return (sum);

}

int bin2mask(char *mask){
	
	int i=0;
	int partial[3];
	int sum=0;
	int mask_int[9];
	
	for(i=0;i<10;i++){
		
		if(mask[i]=='1')
			mask_int[i]=1;
		else
			mask_int[i]=0;
		
	}
	/*
	partial[0]= (1*mask_int[2]) + (2*mask_int[1]) + (4*mask_int[0]);
	partial[1]= (1*mask_int[5]) + (2*mask_int[4]) + (4*mask_int[3]);
	partial[2]= (1*mask_int[8]) + (2*mask_int[7]) + (4*mask_int[6]);
	*/
	
	//printf("%d + %d + %d \n",partial[0],partial[1],partial[2]);
	
	//sum = partial[2] +(10*partial[1])+ (100*partial[0]);
	//sum =(1* mask_int[9])+(2*mask_int[8])+(4*mask_int[7])+(8*mask_int[6])+(16*mask_int[5])+
	 //   (32*mask_int[4])+(64*mask_int[3])+(128*mask_int[2])+(256*mask_int[1])+(512*mask_int[0]);
	/*
	sum =(2* mask_int[9])+(4*mask_int[8])+(8*mask_int[7])+(16*mask_int[6])+(32*mask_int[5])+
		    (64*mask_int[4])+(128*mask_int[3])+(256*mask_int[2])+(512*mask_int[1])+(1024*mask_int[0]);
	*/
	return sum;
	
}

int my_change_mod(char *path, char *binary, char *permission) {

	char mask[9];
	int i=0;
	//8 9 10 - 11 12 13 - 14 15 16
	if (strcmp(permission, "+w")==0) {

		binary[9]='1';
		binary[12]='1';
		binary[15]='1';

		for (i=0; i<9; i++) {

			mask[i]=binary[i+8];
			printf("%c-%c\n", mask[i],binary[i+8]);
		}

		printf("\n--%d--\n",32768+bin2mask(mask));
		
		
		if(chmod(path,(mode_t)bin2mask(mask))==-1)
			return -1;
		else 
			return 0;

	}

}

Resource *my_attrib(char *path) {

	DIR *dp;

	Resource *temp, *to_return=NULL, *to_print=NULL, *temp2;
	struct dirent *ep;
	int p;
	char *command = (char *)malloc((unsigned int)(MAXPATH + 15));
	char *temp_path = (char *)malloc((unsigned int)MAXPATH);
	char *name = (char *)malloc((unsigned int)MAXPATH);

	struct stat status;
	char binary[80];

	strcpy(temp_path, path);

	//if no parameters, print info
	if (r_option + w_option + h_option + x_option == 0) {

		dp = opendir(temp_path);

		//scan for files
		if (dp != NULL) {
			while ( (ep=readdir(dp) )) {

				if (ep->d_name[strlen(ep->d_name)-1] == '~')
					continue;

				strcpy(temp_path, (char *)build_path(path, ep->d_name));

				if ( (p= open(temp_path, O_EXCL)) == -1) {

					fprintf(stderr,"ATTRIB: cannot access : %s: No such file or directory\n",
					temp_path);
					exit(1);
				}

				if (stat(temp_path, &status) != 0) {
					if (ep->d_type==4)
						printf("Cannot open directory %s: Permission denied\n",
								temp_path);
					else
						printf("Cannot open file %s: Permission denied\n",
								temp_path);
					continue;

				}

				strcpy(name, ep->d_name);

				if (strcmp(name, ".")==0 || strcmp(name, "..")==0)
					continue;

				//if hidden, yes
				if (name[0] == '.') {

					temp2 = create_res(status, ep->d_name, ep->d_type, path);
					insert_resource(&to_print, temp2);

				}

				if (d_option==FALSE) {//no folders
					if (! (ep->d_name[0]=='.' || strcmp(name, "..")==0
							|| ep->d_name[strlen(ep->d_name)-1] == '~' || ep->d_type==4)) {
						temp2
								= create_res(status, ep->d_name, ep->d_type,
										path);
						insert_resource(&to_print, temp2);
					}
				} else {//yes folders

					if (! (ep->d_name[0]=='.' || strcmp(name, "..")==0
							|| ep->d_name[strlen(ep->d_name)-1] == '~')) {
						temp2
								= create_res(status, ep->d_name, ep->d_type,
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
						if (strcmp(ep->d_name, "..")==0 || strcmp(ep->d_name,
								"..")==0 || ep->d_name[strlen(ep->d_name)-1] == '~')
							continue;

						else {
							temp = create_res(status, ep->d_name, ep->d_type,
									path);
							insert_resource(&to_return, temp);
							//printf("resource %s inserted, if folder type 4, %d\n",temp->name, temp->type);
						}

						if (d_option==0)//if d option, don't process folder
							continue;

					}

				}

			}

			print_attrib(to_print, path);
		}

		closedir(dp);
		close(p);
	}

	else {

		if ( (p = open(path, O_EXCL)) == -1) {

			fprintf(stderr,"ATTRIB: cannot access : %s: No such file or directory\n", path);
			return NULL;
		}

		if (stat(path, &status) != 0) {
			if (ep->d_type==4)
				printf("Cannot open resource %s: Permission denied\n", path);

			return NULL;

		}

		dec2bin((long)status.st_mode, (char *)&binary);
		//8 9 10 - 11 12 13 - 14 15 16

		//chmod +r d.txt
		//TODO! try to do it with set mode 
		
		
		if (r_option == 1) {
/*
			if (my_change_mod(path, binary, "+r")<0) {

				fprintf(stderr,"ATTRIB: can't change mod, permission denied\n");
				exit(1);

			}

			return to_return;
			*/
			strcpy(command, "chmod +r ");

		}

		if (r_option == 2) {
/*
			if (my_change_mod(path, binary, "-r")<0) {

				fprintf(stderr,"ATTRIB: can't change mod, permission denied\n");
				exit(1);

			}

			return to_return;
*/
			strcpy(command, "chmod -r ");

		}
		if (w_option == 1) {
/*
			if (my_change_mod(path, binary, "+w")<0) {

				fprintf(stderr,"ATTRIB: can't change mod, permission denied\n");
				exit(1);

			}

			return to_return;
*/
			strcpy(command, "chmod +w ");
		}
		if (w_option == 2) {
/*
			if (my_change_mod(path, binary, "-w")<0) {

				fprintf(stderr,"ATTRIB: can't change mod, permission denied\n");
				exit(1);

			}

			return to_return;*/
			strcpy(command, "chmod -w ");
		}
		if (x_option == 1) {
/*
			if (my_change_mod(path, binary, "+x")<0) {

				fprintf(stderr,"ATTRIB: can't change mod, permission denied\n");
				exit(1);

			}

			return to_return;*/
			strcpy(command, "chmod +x ");
		}
		if (x_option == 2) {
/*
			if (my_change_mod(path, binary, "-x")<0) {

				fprintf(stderr,"ATTRIB: can't change mod, permission denied\n");
				exit(1);

			}

			return to_return;*/
			strcpy(command, "chmod -x ");

		}
		
		 strcat(command, path);
		 system(command);
		 
		close(p);
	}

	/*
	 free(command);
	 free(temp_path);
	 free(name);*/

	return to_return;
}

Resource *attrib_processNode(char *path) {

	Resource *to_print=NULL;

	to_print = my_attrib(path);

	//to_dir = print_list(to_print, path, (char *)parameters_global);

	return to_print;

}

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

}

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
				printf("ATTRIB: not valid argument\n");
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

