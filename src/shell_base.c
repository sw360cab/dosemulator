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
#include <sys/wait.h>
#include <fcntl.h>
#include "parse.h"

// execute the command written inside shell
// *** receive command, options *** 
void exec_com(char * command, char * options) {
	param *parameter_list;
	char working_dir[MAXPATH], buf[2];
	char *command2, *options2;
	char *new_dir;
	char *line;
	int status;
	int fd=-1, piped=0;
	pid_t pid;

	parameter_list=parse_options(options, &fd, &piped);

	// command with pipe
	if (piped!=0) {
		// create pipe
		pipe(pipe_comm);
		// fork to distinguish between command before and after pipe
		if ((pid=fork())==0) // first command just open pipe and then exec command normally
		{
			close(1);
			dup(pipe_comm[1]);
			close(pipe_comm[1]);
		} 

		else // second command after pipe -- need to be parsed from the beginning
		{
			// wait end of first command
			waitpid(pid, &status, 0);

			// exit immediately if status is 1 -- something went wrong witn
			if (status!=0) // previous command did not end correctly
				exit(1);

			// set pipe
			close(0);
			dup(pipe_comm[0]);
			close(pipe_comm[0]);

			// retrieve string of command after pipe
			line = pipe_string(options);
			//fprintf(stdout,"PIPE: new line --%s--\n",line);

			command2 = (char *) malloc(sizeof(char)*20);
			options2 = (char *) malloc(sizeof(char)*strlen(line));

			parse_line(&command2, &options2, line);
			strcpy(command, command2);
			parameter_list=parse_options(options2, &fd, &piped);
			//fprintf(stdout,"PIPE:  Found COMMAND ---%s---\n and OPTIONS ---%s---\n", command,options2);
			free(command2);
			free(options2);
		}
	} // end command with pipe

	if (fd>0 && piped==0) // there is a redirection - not valid with first command of a pipe
	{
		close(1);  // close stdout
		dup(fd);   // duplicate fd -> this goes in the entry where there was stdout
		close(2);  // close stdout
		dup(fd);   // duplicate fd -> this goes in the entry where there was stderr
		close(fd); // close original fd
	}

	getcwd(working_dir, BUF_MAX);
	write(current_dir[1], working_dir, strlen(working_dir));

	// switch used to execute commands
	if (strcasecmp(command, "cd")==0 || strcasecmp(command, "chdir")==0) {
		new_dir=cd(working_dir, parameter_list);

		if (strcmp(new_dir, working_dir) != 0) // working directory has changed
		{
			// invalid previous content of pipe
			buf[0]='*';
			write(current_dir[1], buf, 1);
			write(current_dir[1], new_dir, strlen(new_dir));
		}
	} 
	else if (strcasecmp(command, "attrib") == 0)
		attrib(parameter_list);
	else if (strcasecmp(command, "cls") == 0)
		cls(parameter_list);
	else if (strcasecmp(command, "copy")==0)
		cp(parameter_list);
	else if (strcasecmp(command, "xcopy")==0)
		xcp(parameter_list);
	else if (strcasecmp(command, "diskcopy")==0)
		disk_copy(parameter_list);
	else if (strcasecmp(command, "date") == 0)
		date(parameter_list);
	else if (strcasecmp(command, "del")==0 || strcasecmp(command, "erase")==0)
		del(parameter_list);
	else if (strcasecmp(command, "deltree")==0)
		deltree(parameter_list);
	else if (strcasecmp(command, "rd")==0 || strcasecmp(command, "rmdir")==0)
		rd(parameter_list);
	else if (strcasecmp(command, "dir") == 0)
		dir(parameter_list);
	else if (strcasecmp(command, "echo") == 0)
		echo(parameter_list);
	else if (strcasecmp(command, "fc") == 0)
		fc(parameter_list);
	else if (strcasecmp(command, "find") == 0)
		find(parameter_list);
	else if (strcasecmp(command, "help")==0)	
		help(parameter_list);
	else if (strcasecmp(command, "list")==0)
		list(parameter_list);
	else if (strcasecmp(command, "md")==0 || strcasecmp(command, "mkdir")==0)
		md(parameter_list);
	else if (strcasecmp(command, "more") == 0)
		more(parameter_list);
	else if (launch_exe(command, parameter_list)<0) {
		fprintf(stderr,"Error: \'%s\' unknown or bad typed command\n", command);
		fprintf(stderr,"Try \'help\' or \'list\' for further information\n");
		exit(1);
	}

	my_free(&parameter_list);
	exit(0);
}

int main(int argc, char **argv) {
	int status;
	int running= TRUE;
	char *command, *opt;
	char *line;
	char *working_dir, *new_dir;
	char buf[MAXPATH];
	int length;

	fprintf(stdout,"\n-- This is Microsoft Windows DOS shell emulator 1.0\n");
	fprintf(stdout,"-- you can type windows command or run programs\n");
	fprintf(stdout,"-- type \'list\' for a list or \'help command\' for help\n");
	fprintf(stdout,"-- otherwise type \'quit\' or \'exit\' to quit\n\n");

	working_dir = (char *) malloc(sizeof(char)*BUF_MAX);
	new_dir = (char *) malloc(sizeof(char)*BUF_MAX);
	getcwd(working_dir, BUF_MAX);

	// create pipe
	pipe(current_dir);

	while (running) {
		fprintf(stdout,"$$-WinShell-$$:%s >> ", working_dir);

		// get_line
		line=get_line();

		// parse command
		command = (char *) malloc(sizeof(char)*20);
		opt = (char *) malloc(sizeof(char)*strlen(line));

		parse_line(&command, &opt, line);

		// fprintf(stdout,"Found COMMAND ---%s---\n and OPTIONS ---%s---\n", command,opt);

		if (strcasecmp(command, "exit")==0 || strcasecmp(command, "quit")==0) {
			running=FALSE;

			fprintf(stdout,"\n !! Bye Bye !!\n\n");
			free(command);
			free(opt);
			break;
		}

		if (fork() == 0) // child
		{
			//execl("/bin/ls", "ls", "-l", (char *)0);
			exec_com(command, opt);
			free(command);
			free(opt);
		} else // father
		{
			wait(&status);

			// routines to detect if need to move to new dir
			length=read(current_dir[0], buf, BUF_MAX);
			buf[length]='\0';

			// buffer contains onvalidating char '*'
			if ( (new_dir=strrchr(buf, '*')) != NULL) // working directory has changed in the child
			{
				chdir(new_dir+1);
				//fprintf(stdout,"New dir --%s--\n",new_dir+1);

				getcwd(working_dir, BUF_MAX);
			}
		}
	}
	return 0;
}
