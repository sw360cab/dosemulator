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

#define TRUE 1
#define FALSE 0

void exec_com (char * buf)
{
	printf("%s executed\n", buf);
	exit(0);
}

int main(int argc,char **argv) 
{
	int status;
	int running = TRUE;
	char command[50];
	
	printf ("\n-- This is Windows DOS shell emulation 1.0\n");
	printf ("-- you can type windows command or run programs\n");
	printf ("-- type 'help' for a list of command or 'exit' to quit\n\n");
	
	while (running)
	{
		printf("$$-WinShell-$$ >> ");
		// get_line
		// parse command
		scanf("%s", command);
		
		if ( strcmp(command,"exit")==0 || strcmp(command,"quit")==0 )
		{
			running=FALSE;
			break;
		}
		
		if (fork() == 0)	// child
			//execl("/bin/ls", "ls", "-l", (char *)0);
			exec_com(command);
		else	// father
			wait(&status);
	}
}

/*
 * commad type:
 * - help --> stampa "help"
 * - exit --> esce
 * - command --> esegue relativo comando / funzione
 * - unknown --> stampa "WinShell: 'command' - command not found "
 */
 
