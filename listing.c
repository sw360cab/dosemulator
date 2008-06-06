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

void list(param *list_par)
	{
	char src[10];
	char *buf;
	int source_fd;
	int rd;
	struct stat st;
	
	if (list_par != NULL)
		fprintf(stderr, "Warning: \'list\' command does not requires parameters \n\n");
	
	strcpy(src,"list.txt");
   	if ( (source_fd=open (src, O_RDONLY)) == -1)
   		{
   		fprintf(stderr, "Problem with \'list\' command\n");
   		exit(1);
   		}
	
   	while ((rd = read( source_fd, buf, BUF_MAX+1)) > 0 )
   	    	write(1, buf, rd );
   	
   	//free(buf);
   	close(source_fd);
	}
