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
//BUG! if space after argument, it exits
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include "parse.h"
#include "resource.h"

extern void dec2bin(long, char*);

int launch_exe(char *command_path, param *parameters) {

	param *par = parameters;
	struct stat stat_result;
	pid_t pid;
	int i=0, j=1, status;
	int fd;
	char **arguments;
	char binary[80];


	//count the number of arguments(whose length is >0)
	while (par != NULL) {

		
		if(strlen(par->name)>0)
			i++;
		par=par->next;

	}
	
	if (i!=0) {

		par = parameters;

		arguments= (char **) malloc((i+1) * sizeof *arguments);

		//bacause execve is missing the first argument
		arguments[0] = (char *)malloc(2);
		strcpy(arguments[0],"**");
		
		while (par != NULL) {

			if(strlen(par->name) != 0){
				arguments[j]=(char *) malloc(sizeof(char) * strlen(par->name+1));
				strcpy(arguments[j], par->name);
				j++;
			}
			
			par=par->next;
		}
	} else
		arguments = NULL;

	
	if ((fd = open(command_path, O_EXCL))!= -1) {

		if (stat(command_path, &stat_result) == -1) {
			fprintf(stderr, "%s : denied permission\n", command_path);
			return -1;
		}

		dec2bin((long)stat_result.st_mode, (char *) &binary);

		//8 9 10 - 11 12 13 - 14 15 16

		if (! (binary[10]=='1' || binary[13]=='1' || binary[16]=='1')) {

			fprintf(stderr, "%s: any executable permission\n", command_path);
			return -1;
			
		}

		if ((pid=fork())==0) {//child

			execv(command_path, arguments);

		} else { //father
			waitpid(pid,&status,0);
			
			return 0;
		}

	} else {
		
		return -1;

	}
		return 0;
}
