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
#define BUF 512;

short redirection_out= FALSE;
short redirection_in= FALSE;
short append_out= FALSE;
short append_in= FALSE;

void my_echo(char *msg, char *path) {

	FILE *file;
	int fd;


	if (redirection_out==FALSE && append_out==FALSE) {

		printf("%s\n", msg);

	} else if (redirection_out == TRUE) {

		printf("opening %s\n",path);
		file = fopen(path, "w+");

		if (file == NULL) {

			fprintf("ECHO: error opening %s\n", path);
			exit(1);
		}

		fprintf(file, "%s", msg); /*writes*/
		fclose(file); /*done!*/

	} else if (append_out == TRUE) {
		
		file = fopen(path, "a+");

		printf("opening %s\n",path);
		if (file == NULL) {

			fprintf("ECHO: error opening %s\n", path);
			exit(1);
		}

		fprintf(file, "%s", msg); /*writes*/
		fclose(file); /*done!*/
	}

}

void echo(param **paramaters) {

	param *p = (*paramaters);
	char *message, *path;
	short int count = 0;

	while (p!= NULL) {

				
		//maximum three paramters
		if (count> 2) {
			fprintf(stderr,"ECHO : too many arguments\n");
			exit(1);
		}

		if (count == 0) {

			message = (char *) malloc((unsigned short)strlen(p->name));
			strcpy(message, p->name);

		} else if (count==1) {

			if (strcasecmp(p->name, ">") == 0)
				redirection_out=TRUE;
			else if (strcasecmp(p->name, ">>") == 0)
				append_out = TRUE;

			else {
				fprintf(stderr,"ECHO : not valid argument\n");
				exit(1);
			}

		} else if (count==2) {
			if (p->type==0) {

				path=(char *)malloc((unsigned short)strlen(p->name));
				strcpy(path,p->name);
			}

			else {
				fprintf(stderr,"ECHO : not valid path\n");
				exit(1);
			}
		}

		p=p->next;
		count++;
	}

	if (count==2) {
		fprintf(stderr,"ECHO: wrong number of arguments\n");
		exit(1);
	} else if (count == 1) {
		my_echo(message, NULL);
	} else if (count == 3) {
		my_echo(message, path);
	}
}
