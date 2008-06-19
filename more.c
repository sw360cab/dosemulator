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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parse.h"
//TODO use lioy function to read and not fgets

short int c_option= FALSE;
short n_option=FALSE;
short int s_option = FALSE;

void my_more(char *src) {

	char *buf;
	int row=0, space = 0;

	FILE *fp;

	if ( (fp = fopen(src, "r")) == NULL) {
		fprintf(stderr,"MORE: cannot access : %s: No such file or directory\n",
		src);
		//exit(1);
		return;
	}

	buf = (char *) malloc((unsigned int)STRING_LENGTH);

	if (c_option == TRUE)
		system("clear");

	while (fgets(buf, (int)STRING_LENGTH, fp) != NULL) {

		if (n_option>0 && row<n_option) {
			row++;
			continue;
		}
		
		if(s_option == TRUE){
			if(strlen(buf) == 1)
				space ++;
			else space=0;
			
			if(space > 1){
				row++;
				continue;
			}
				
		}
		fprintf(stdout,"%s",buf);

		row++;
	}
	fprintf(stdout,"\n");
	free(buf);

}

void more(param *parameters) {

	param *p = parameters;
	short int files = 0;
	
	if (p==NULL) {
			fprintf(stderr, "md: missing file operand\n");
			fprintf(stderr, "Try \'help md\' for more information\n");
			exit(1);
		}
	

	while (p!= NULL) {

		if (p->type==1) {

			if (strcasecmp(p->name, "\\C") == 0)
				c_option=TRUE;

			else if (p->name[0] == '+')
				n_option=atoi(p->name);

			else if (strcasecmp(p->name, "\\S") == 0)
				s_option=TRUE;

			else {
				fprintf(stdout,"MORE : not valid argument\n");
				exit(1);
			}
		}

		p=p->next;
	}

	p = parameters;

	while (p!= NULL) {

		if (p->type==0) {

			if (files>0) {
				fprintf(stdout,"MORE: Press ENTER to continue to the next file...\n");
				getchar();
			} 

			my_more(p->name);
			files++;

		}

		p=p->next;
	}

}

