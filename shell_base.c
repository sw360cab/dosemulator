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

void exec_com (char * command, char * options)
{
	param *parameter_list;
	char working_dir[MAXPATH], buf[2];
	char *new_dir;
	
	parameter_list=parse_options(options);
	
	/*
	while (parameter_list!=NULL)
	{
		printf("---%s---\n", parameter_list->name);
		parameter_list= parameter_list->next;
	}
	*/
	
	getcwd(working_dir,BUF_MAX);
	write(current_dir[1],working_dir,strlen(working_dir));
	
	// switch used to execute commands
	/*if (strcmp(command,"copy")==0 )
		cp(&parameter_list);
	else*/ if (strcmp(command,"cd")==0 )
		{
		new_dir=cd(working_dir,&parameter_list);
		
		if(strcmp(new_dir,working_dir) != 0 ) // working directory has changed
			{
			// invalid previous content of pipe
			buf[0]='*';
			write(current_dir[1],buf,1);
			write(current_dir[1],new_dir,strlen(new_dir));			
			}
		}
	else if (strcmp(command,"md")==0 )
		md(parameter_list);
	else if (strcmp(command,"del")==0 )
		del(parameter_list);
	else if (strcmp(command,"deltree")==0 )
		deltree(parameter_list);
	else if (strcmp(command,"list")==0 )
		list(parameter_list);
	else if (strcmp(command,"help")==0 )
		list(parameter_list);
	else
		{
		fprintf(stderr,"Error: \'%s\' unknown or bad typed command\n", command);
		fprintf(stderr,"Try \'help\' or \'list\' for further information\n");
		}

	// TODO check free
	//free(parameter_list);
	exit(0);
}

int main(int argc,char **argv) 
{
	int status;
	int running = TRUE;
	char *command, *opt;
	char *line;
	char *working_dir, *new_dir; 
	char buf[MAXPATH];
	int length;
	
	printf ("\n-- This is Windows DOS shell emulation 1.0\n");
	printf ("-- you can type windows command or run programs\n");
	printf ("-- type 'help' for a list of command or 'exit' to quit\n\n");
	
	working_dir = (char *) malloc(sizeof(char)*BUF_MAX);
	new_dir = (char *) malloc(sizeof(char)*BUF_MAX);
	getcwd(working_dir,BUF_MAX);
	
	// create pipe
	pipe(current_dir);
	
	while (running)
	{		
		printf("$$-WinShell-$$:%s >> ", working_dir);
		
		// get_line
		line=get_line();
		
		// parse command
		command = (char *) malloc(sizeof(char)*20);
		opt = (char *) malloc(sizeof(char)*strlen(line));

		parse_line(&command, &opt, line);
		
		printf("Trovati COMANDO ---%s---\n e OPZIONI ---%s---\n", command,opt); 

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
			close(current_dir[0]);
			exec_com(command,opt);
			free(command);
			free(opt);
		}
		else 	// father
		{
			wait(&status);
			
			length=read(current_dir[0],buf,BUF_MAX);
			buf[length]='\0';
			//printf("--pipe--%d--%s--\n",length,buf);

			// buffer contains onvalidating char '*'
			if( (new_dir=strrchr(buf,'*')) != NULL ) // working directory has changed in the child
				{
				chdir(new_dir+1);
				printf("New dir --%s--\n",new_dir+1);
				
				getcwd(working_dir,BUF_MAX);
				}
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
 
