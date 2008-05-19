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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

#define TRUE 1
#define FALSE 0

extern char *get_line();
extern void cp(param**);

void exec_com (char * command, char * options)
{
	param *parameter_list;
		
	parameter_list=parse_options(options);
	
	/*
	while (parameter_list!=NULL)
	{
		printf("---%s---\n", parameter_list->name);
		parameter_list= parameter_list->next;
	}
	*/
	
	// switch used to execute commands
	if (strcmp(command,"copy")==0 )
		cp(&parameter_list);
		
	free(parameter_list);
	exit(0);
}

int main(int argc,char **argv) 
{
	int status;
	int running = TRUE;
	char *command, *opt;
	char *line;
	
	printf ("\n-- This is Windows DOS shell emulation 1.0\n");
	printf ("-- you can type windows command or run programs\n");
	printf ("-- type 'help' for a list of command or 'exit' to quit\n\n");
	
	while (running)
	{
		printf("$$-WinShell-$$ >> ");
		
		// get_line
		line=get_line();
		
		// parse command
		command = (char *) malloc(sizeof(char)*20);
		opt = (char *) malloc(sizeof(char)*strlen(line));

		parse_line(&command, &opt, line);
		
		printf("Trovati COMANDO ---%s---\n e OPZIONI ---%s---\n", command,opt); 
		
		// TODO invetigate Segmentation fault on "exit
		if ( strcmp(command,"exit")==0 || strcmp(command,"quit")==0 )
		{
			running=FALSE;
			free(command);
			free(opt);
			break;
		}
		
		if (fork() == 0)	// child
		{
			//execl("/bin/ls", "ls", "-l", (char *)0);
			exec_com(command,opt);
			free(command);
			free(opt);
		}
		else 	// father
		{
			wait(&status);
		}
	}
}

/*
 * commad type:
 * - help --> stampa "help"
 * - exit --> esce
 * - command --> esegue relativo comando / funzione
 * - unknown --> stampa "WinShell: 'command' - command not found "
 */
 
