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

#define BUF_MAX 1024
//void help(param *list_par)
int main(int argc, char **argv )
	{
	char src[10],src2[10],src3[10];
	char *buf;
	int source_fd;
	int rd;
	struct stat st;
	
//	if (list_par == NULL)
//		{
//		fprintf(stderr, "Type help \'command\'\n");
//		exit(1);
//		}
//	
//	if (list_par->next != NULL)
//		fprintf(stderr, "Warning: \'list\' command does not requires parameters \n\n");

	if (strcmp(argv[1],"dir")==0)
		strcpy(src,"list.txt");
	else if (strcmp(argv[1],"attrib")==0)
		strcpy(src,"list.txt");
	else if (strcmp(argv[1],"md")==0)
		strcpy(src,"list.txt");
	else if (strcmp(argv[1],"del")==0)
			strcpy(src,"list.txt");
	else if (strcmp(argv[1],"deltree")==0)
			strcpy(src,"list.txt");
	else if (strcmp(argv[1],"copy")==0)
			strcpy(src,"list.txt");
	else if (strcmp(argv[1],"xcopy")==0)
			strcpy(src,"list.txt");
	else
		{
	   	fprintf(stderr, "Unknown command - type \'list\' for all available commands\n");
		exit(1);
		}
	
	// easier to concatenated but it doesn't work at all
	//	strcpy(src2,"list");
	//	strcpy(src3,".txt");
	//	sprintf(src,"%s.txt",src2);
	
	strcpy(src2,src);
   	if ( (source_fd=open (src2, O_RDONLY)) == -1)
   		{
   		fprintf(stderr, "Problem with \'help\' command\n");
   		exit(1);
   		}
   	

   	/*while ((rd = read( source_fd, buf, (BUF_MAX/2)+1)) > 0 )
   	    	write(1, buf, rd);
*/
   	rd = read( source_fd, buf, (BUF_MAX/2)+1);
   	printf("%s+++%d\n",src2,rd);
   	 write(1, buf, rd);
   	//free(buf);
   	close(source_fd);
	}
