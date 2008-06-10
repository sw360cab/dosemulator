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

/*
 * 
 * * 
 * TODO: menage duoble quotes in the path ""
 * TODO: update the current working dir
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "parse.h"
/*
 **  Function : void get_input(char *format,void *variable)
 **  Return   : None
 **  Remarks  : To be used instead of scanf. 'format' is a scanf format,
 **             'variable' the pointer to the variable in which the input
 **             is written.
 */

char *cd(char *current, param **list) {
	//TODO handle paramteres
	param *p = (*list);
	short int flag= FALSE;
	char *path;

	while (p!=NULL && flag==FALSE) {

		if (p->type==0) {

			path = (char *)malloc((unsigned int)strlen(p->name));
			strcpy(path, p->name);
			flag = TRUE;

		}
		else if(p->type==1){
			
			fprintf(stderr, "CD: invald argument\n");
			exit(1);
			
		}
		p=p->next;

	}

	if (flag == FALSE) {
		path = (char *)malloc(2);
		strcpy(path, ".");
	}

	if (chdir(path) != 0) {
		printf("CD: Impossible to find path\n");
		free(path);
		return current;
	} else {
		printf("%s\n", path);
		return path;
	}
}

