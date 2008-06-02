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

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include "parse.h"
#include "resource.h"

//options: 0 deafult, 1=+=put attribute,2=-=remove attribute
short int r_option=0;
short int w_option=0;
short int x_option=0;
short int h_option=0;

//S and D to use without previous arguments
short int s_parameter=0; //S is recursion only if used without arguments 
short int d_option=0; //d valid only if s_parameter is 1;


struct stat st;

//TODO S,D,H
//TODO no arguments
void my_attrib(char *path) {

	DIR *dp;
	struct dirent *ep;
	int *p;
	char *command = (char *)malloc((unsigned int)(MAXPATH + 15));
	char *temp_path = (char *)malloc((unsigned int)MAXPATH);
	int mask[4]= { 0, 0, 0, 0 };//R,W,X,H
	struct stat status;

	strcpy(temp_path, path);

	//if no parameters, print info
	if (r_option + w_option + h_option + x_option == 0) {

		if (s_parameter == 0) {

			dp = opendir(temp_path);

			if (dp != NULL) {
				while ( (ep=readdir(dp) )) {

					if (ep->d_type == 4)
						continue;

					if ( (p=(int *)open(temp_path, O_EXCL)) == NULL) {

						fprintf(stderr,"ATTRIB: cannot access : %s: No such file or directory\n",
						temp_path);
						exit(1);
					}

					if (fstat((int)p, &status) != 0) {
						if (ep->d_type==4)
							printf(
									"Cannot open directory %s: Permission denied\n",
									temp_path);
						else
							printf("Cannot open file %s: Permission denied\n",
									temp_path);
						continue;

					}
					
					if((((unsigned short)status.st_mode) == 33024))
						mask[0]=1;
						
					
					if (ep->d_name[0]=='.')
						
						mask[3]=1;

				}

			}

		} else {
			if (d_option == FALSE) {

			} else {

			}

		}

	}

	if ( (p = (int *)open(path, O_EXCL)) == NULL) {

		fprintf(stderr,"ATTRIB: cannot access : %s: No such file or directory\n", path);
		return;
	}

	//chmod +r d.txt

	if (r_option == 1)
		strcpy(command, "chmod +r ");

	if (r_option == 2)
		strcpy(command, "chmod -r ");

	if (w_option == 1)
		strcpy(command, "chmod +w ");

	if (w_option == 2)
		strcpy(command, "chmod -w ");

	if (x_option == 1)
		strcpy(command, "chmod +x ");

	if (x_option == 2)
		strcpy(command, "chmod -x ");

	strcat(command, path);
	system(command);

}

void attrib(param **parameters) {

	param *p = (*parameters);
	short int files = 0;
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
		fprintf(stderr,"Option \\D is valid only if combinated with option \\S");
	}

	p = (*parameters);

	//second: search for paths
	while (p!= NULL) {

		if (p->type==0) {
			c++;
			my_attrib(p->name);

		}

		p=p->next;
	}

	//if no paths
	if (c==0) {
		current_dir = (char *) malloc((unsigned int)MAXPATH);
		getcwd(current_dir, MAXPATH);

		my_attrib(current_dir);

	}

}
