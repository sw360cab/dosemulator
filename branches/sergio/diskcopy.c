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
#include <sys/sysmacros.h>
#include <dirent.h>
#include <fcntl.h>
#include "parse.h"

void disk_copy(param* list)
{
	char path[10], *buf, *src;
	int source_fd, floppy_fd;
	int written_bytes=0;
	int rd, stat_res;
	struct stat st;

	buf= (char*)malloc(sizeof(char)*BUF_MAX);

	strcpy(path,"floppy.out");

	// dev file major-minor numbers --> http://www.lanana.org/docs/device-list/devices.txt
	
	// if device does not exist create it
	if ( (floppy_fd=open (path, O_WRONLY)) == -1)
	{
		if ( mknod(path, S_IFBLK | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, makedev(2,28)) == -1)
			{
			fprintf(stderr, "Can't find floppy device %s\n",path);
			exit(1);
			}
		close(floppy_fd);
	}

	// try to open floppy dev file
	if ( (floppy_fd=open (path, O_WRONLY)) == -1)
	{
		fprintf(stderr, "Can't access floppy file %s\n",path);
		exit(1);
	}

	while (list != NULL){

		// take info about file / dir
		stat_res=stat(list->name,&st);

		if ( list->type==0 && S_ISREG(st.st_mode) && stat_res>=0) // source is a file
		{

			src = (char *) malloc(sizeof(char)*strlen(list->name)+1);
			strcpy(src,list->name);

			// try to open source file
			if ( (source_fd=open (src, O_RDONLY)) == -1)
			{
				fprintf(stderr, "Can't open source file %s\n",src);
				exit(1);
			}

			// file to write does not exceed floppy size 
			if (written_bytes + st.st_size < 1440)
			{
				while ((rd = read( source_fd, buf,BUF_MAX)) > 0 )
					write(floppy_fd, buf, rd /*BUF_MAX*/ );

				// increase writte bytes
				written_bytes += st.st_size;

				close(source_fd);
			}

			else  //not enough space on floppy
			{
				fprintf(stderr, "Error - not enough space on floppy\n");
				close(source_fd);
				exit(1);
			}	


		}

		else // no option for this command
		{
			fprintf(stderr, "%s - unknown option\n",list->name );
			exit(1);
		}
		
		 //free space
		if (strlen(src)!=0)
			free(src);
		
		list = list->next;
	} // end while
	close(floppy_fd);
}
