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

/*
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

		fprintf(file, "%s", msg); 
		fclose(file);

	} else if (append_out == TRUE) {
		
		file = fopen(path, "a+");

		printf("opening %s\n",path);
		if (file == NULL) {

			fprintf("ECHO: error opening %s\n", path);
			exit(1);
		}

		fprintf(file, "%s", msg); 
		fclose(file); 
	}

}
*/

void echo(param *paramaters) {

	param *p;
	char *message;
	short int count = 0;

	p=paramaters;
	
	if( p == NULL ){
		printf("\n");
	}
	
	while (p!= NULL) {

		printf("%s ",p->name);
		p=p->next;
	}
	printf("\n");
}
