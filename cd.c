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
#include "parse.h"

/*
 * change the current directory
 * *** receive current directory, list with path and options - return new current dir ***
 */
char *cd(char *, param *);


/*
 * change the current directory
 */
char *cd(char *current, param *list) {

	param *p = list;
	short int flag= FALSE, i;
	char *path;
	char *doublequotes, *temp;


	while (p!=NULL && flag==FALSE) {

		if (p->type==0) {

			path = (char *)malloc((unsigned int)strlen(p->name));
			strcpy(path, p->name);
			flag = TRUE;

		} else if (p->type==1) {

			fprintf(stderr, "CD: invald argument\n");
			exit(1);

		}
		p=p->next;

	}

	if(flag==TRUE && p != NULL){

		fprintf(stderr,"CD: too many arguments\n");
		exit(1);
	}


	if (flag == FALSE) {
		path = (char *)malloc(2);
		strcpy(path, ".");
	}

	//if double quotes both at the beginning and the end,remove them
	if (path[0]=='"' && path[strlen(path)-1]=='"') {

		doublequotes = (char *)malloc(strlen(path)-2);
		temp = (char *)malloc(1);

		for (i=1; i<strlen(path)-1; i++) {

			sprintf(temp, "%c", path[i]);
			if (i==1)
				strcpy(doublequotes, temp);
			else
				strcat(doublequotes, temp);
		}

		strcpy(path, doublequotes);
		free(temp);
		free(doublequotes);
	}

	if (chdir(path) != 0) {
		fprintf(stdout,"CD: Impossible to find path %s\n",path);
		free(path);
		return current;
	} else {
		//fprintf(stdout,"%s\n", path);
		return path;
	}
}

