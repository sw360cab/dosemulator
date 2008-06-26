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
#include <dirent.h>
#include <fcntl.h>
#include "parse.h"

// PROTOTYPES

// help for a specific command
void help(param *);
// list available command
void list(param *);

// help for a specific command
void help(param *list_par)
{
	char *src;
	char *buf;
	int source_fd;
	int rd;

	if (list_par == NULL)
	{
		list(list_par);
		fprintf(stderr, "Type help \'command\'\n\n");
		exit(1);
	}

	if (list_par->next != NULL)
		fprintf(stderr, "Warning: only a command help request can be satisfied a time\n\n");

	src=(char*)malloc(sizeof(char)*strlen(list_par->name)+12);
	strcpy(src, list_par->name);

	sprintf(src,"helper/%s.txt",list_par->name);
	
	// allocate buffer for copying
	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);

	if ( (source_fd=open (src, O_RDONLY)) == -1)
	{
		fprintf(stderr, "Problem with \'help\' command\n");
		exit(1);
	}

	while ((rd = read( source_fd, buf, BUF_MAX)) > 0 )
		write(1, buf, rd);
	fprintf(stdout, "\n");
	
	close(source_fd);
	free(buf);
}

// list available command
void list(param *list_par)
{
	char *src;
	char *buf;
	int source_fd;
	int rd;

	if (list_par != NULL)
		fprintf(stderr, "Warning: \'list\' command does not requires parameters \n\n");

	src=(char*)malloc(sizeof(char)*16);
	strcpy(src,"helper/list.txt");
	if ( (source_fd=open (src, O_RDONLY)) == -1)
	{
		fprintf(stderr, "Problem with \'list\' command\n");
		exit(1);
	}
	
	// allocate buffer for copying
	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);

	while ((rd = read( source_fd, buf, BUF_MAX)) > 0 )
		write(1, buf, rd );
	fprintf(stdout, "\n");

	close(source_fd);
	free(buf);
}

// help function version with file pointer
/*
void help(param *list_par)
{
	char *src;
	char *buf, c;
	int source_fd;
	int rd;
	FILE *fp;

	if (list_par == NULL)
	{
		fprintf(stderr, "Type help \'command\'\n");
		exit(1);
	}

	if (list_par->next != NULL)
		fprintf(stderr, "Warning: only a command help request can be satisfied a time\n\n");

	src=(char*)malloc(sizeof(char)*strlen(list_par->name)+5);
	strcpy(src, list_par->name);
	
	// concatenated doesn't work at all with file pointers
	sprintf(src,"%s.txt",list_par->name);

	if ((fp = fopen(src, "r"))==NULL)
	{
		fprintf(stderr,"Error: \'%s\' unknown or bad typed command, try \'list\' for a list of commands\n", src);
		exit(1);
	}
	while ((c = getc (fp)) != EOF)
		putchar(c);

	fclose(fp);
}*/
