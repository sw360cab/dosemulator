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

// global option flag
// set all to 0 !!!
int verify=0, override=0;
int confirm=1, subdir=1, empty=0;
int show_name=0, no_copy=1, no_file=1, overread=0;

void err ()
{
	fprintf(stderr, "Error - check number or type of arguments,\nalternatively name or permissions of files\n");
	exit(1);
}

// check if the source file is a concatenation --> contains '+'
int concat (char *files)
{
	struct stat st;

	if (strchr(files,'+')!=NULL && stat(files,&st)!=0 )
		return TRUE;
	else
		return FALSE;
}

// create the list of files in case of a concatetanation
param* create_source(char *files)
{
	int end, count=0;
	int source_fd;
	param *p;
	param *pt=NULL;

	do
	{
		while ( strncmp( files+count, "+",1)!=0 && strncmp( files+count,"\0",1)!=0 )
			count++ ;
		end=count;
		p=new_elem();

		p->name = (char *) malloc(sizeof(char)* (end) );
		strncpy(p->name, files, end);

		// try to open file to verify exsistance	
		if ( (source_fd=open (p->name, O_RDONLY)) == -1)
		{
			fprintf(stderr, "Can't open source file %s\n",p->name);
			exit(1);
		}
		close(source_fd);

		printf("%s to be parsed\n", p->name);
		p->type = 0;
		p->next=NULL;

		insert_e(&pt,p);

		files+=count+1;
		count=0;
	}while ( strncmp( files, "\0",1)!=0 );
	return pt;	
}

//ask confirmation to override existing dest file
int req_overridden(char *pth)
{
	char *answ;

	while(1)
	{
		printf ("File %s already exists, Do you want to overwrite it? [Yes or No]\n", pth);

		answ=get_line();
		if (strncmp(answ,"yes",3)==0 || strncmp(answ,"YES",3)==0 || strncmp(answ,"Yes",3)==0 )
			return 1;
		else if ( strncmp(answ,"y",1)==0 || strncmp(answ,"Y",1)==0 )
			return 1;
		else	// NO
		{
			if (strncmp(answ,"no",2)==0 || strncmp(answ,"NO",2)==0 || strncmp(answ,"No",2)==0)
				return 0;
			else if ( strncmp(answ,"n",1)==0 || strncmp(answ,"N",1)==0 )
				return 0;
			else 
				fprintf(stderr, "Type Yes or No !\n");
		}
	}
}

// check if destination file exists
// in case ask user what to do
int overridden(char *dest)
{
	int dest_fd;

	if ( (dest_fd=open (dest, O_RDONLY)) > 0)  // dest file exist
	{
		if ( req_overridden(dest) == 0 )  // ask user what to do
		{
			close(dest_fd);
			return FALSE;
		}
		close(dest_fd);
		return TRUE;
	}
}

// copy files
void cp(param *list)
{ 
	char *buf, *temp_path, c;
	char *src, *dest;
	char *working_dir, *src_dir;
	int source_fd, dest_fd;
	int rd;
	int stat_res;
	param *files=NULL;
	struct stat st, st_dest;
	struct dirent *ep;
	DIR *dp;
	off_t count_concat=0;

	// global
	// int verify=0, override=0;
	int flag=0, cat=0, dir=0;


	if(list==NULL)
	{
		fprintf(stderr, "copy: missing file operand\n");
		fprintf(stderr, "Try \'help copy\' for more information\n");
		exit(1);
	}

	/* // copy for pure text file
	 * 
	 * FILE *in, *out;
	 * in = fopen(“wordlist.txt”, “r”);
	 * out = fopen(“copy.txt”, “w”);
	 * while ((c = getc (in)) != EOF) {
	 * 	putc(c,out);
	 * }
	 * fclose(in);
	 */

	// scroll list of parameters
	while (list!=NULL)
	{
		printf("---%s---\n", list->name);
		if ( list->type==0 ) //path
		{
			// take info about file / dir
			stat_res=stat(list->name,&st);

			if (flag==0) // source
			{
				flag=1;

				if ( concat(list->name) )  // source are files concatenated
				{
					cat=1;
					files=create_source(list->name);
					if (files==NULL)
						err();
				}
				else if ( S_ISDIR(st.st_mode) && stat_res>=0 ) // source is a directory
				{
					dir=1;
					if (opendir( list->name ) == NULL)
						err();
					src_dir = (char *) malloc(sizeof(char)*strlen(list->name)+1);
					strcpy(src_dir,list->name);
				}
				else if ( S_ISREG(st.st_mode) && stat_res>=0) // source is a file
				{
					src = (char *) malloc(sizeof(char)*strlen(list->name)+1);
					strcpy(src,list->name);

					// try to open source file
					if ( (source_fd=open (src, O_RDONLY)) == -1)
					{
						fprintf(stderr, "Can't open source file %s\n",src);
						exit(1);
					}
				}
				else 
					err();

			}
			else if (flag==1) // destination
			{
				flag=2;

				if ( S_ISDIR(st.st_mode) && stat_res>=0) // dest is a directory
				{
					if (dir==1)  // dest dir is valid only if source is a dir
					{
						if (opendir( list->name ) == NULL)
							err();
						dest = (char *) malloc(sizeof(char)*strlen(list->name)+1);
						strcpy(dest,list->name);
					}
					else
					{
						fprintf(stderr, "Error, source is a file and destination is a directory\n");
						exit(1);
					}
				}	
				else // if ( S_ISREG(st.st_mode) )  // dest is a file
				{
					if (dir==1) // dest file is not valid if source is a dir
					{
						fprintf(stderr, "Error, source is a directory and destination is a file\n");
						exit(1);
					}
					else
					{
						dest = (char *) malloc(sizeof(char)*strlen(list->name)+1);
						strcpy(dest,list->name);
					}
				}
			}
			// flag==2 -- too much parameters
			else
				err();	
		}

		else if ( list->type==1 ) //opt
		{	
			//if ( strcmp(list->name,"\A") ||  strcmp(list->name,"\B") )
			if ( strcasecmp(list->name,"\V") )
				verify=1;
			else if ( strncasecmp(list->name,"\-Y",3) )
				override=0;
			else if ( strcasecmp(list->name,"\\Y") ){

				override=1;
				printf(" here\n");
			}
			else
			{
				fprintf(stderr, "%s - unknown option\n",list->name );
				exit(1);
			}
		}
		else
			err();

		list= list->next;
	} // end of while - parsing line of parameters

	// check that enough resources were specified
	if (flag<2) 
		err();

	// allocate buffer for copying
	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);

	// copy files inside directory ignoring subdirectories
	if (dir==1) // directory copy
	{
		// copy all the files of a directory into the new directory		
		dp = opendir(src_dir);
		temp_path = (char *) malloc( sizeof(char)*MAXPATH+1);

		working_dir =(char *) malloc(sizeof(char)*MAXPATH+1);
		getcwd(working_dir,MAXPATH);

		if (dp != NULL) 
		{
			while ( (ep = readdir(dp)) )
			{				
				chdir(src_dir);
				// resource is a file
				if ( ep->d_type==8 && (strcmp(ep->d_name, ".") != 0 ) && (strcmp(ep->d_name, "..")!= 0))
				{	
					// source file created

					// move to source dir
					src=realloc(src, sizeof(char)*strlen(ep->d_name)+1);
					strcpy(src, ep->d_name);
					stat(src,&st);

					// open source file in read mode
					if ( (source_fd=open (src, O_RDONLY)) == -1)
					{
						fprintf(stderr, "Can't open source file %s\n",src);
						exit(1);
					}

					// back to parent dir to open dest dir-file
					chdir(working_dir);

					// create file in dest directory
					strcpy(temp_path,dest);

					//if the current dir doesn't finish with '/',
					//I should add it before appending the currente resource name
					if ( dest[strlen(dest)-1] != '/')
						strcat(temp_path, "/");				
					strcat(temp_path, ep->d_name);

					// override not allowed
					if (override==0 && !overridden(temp_path))
						continue; // go to next file

					if (  (dest_fd=open (temp_path, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
					{
						fprintf(stderr, "Can't create dest file %s\n",temp_path);
						exit(1);
					}

					while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
						write( dest_fd, buf, rd );

					// verify
					if (verify==1)
					{
						stat(temp_path,&st_dest);
						if (st.st_size == st_dest.st_size) // problems with copy -> if files of different sizes
							fprintf(stdout,"Source file %s succesfully copied\n", temp_path);
					}
					fprintf(stdout,"File %s/%s succesfully copied\n", src_dir, src);
					close(source_fd);
					close(dest_fd);
				}
			}
		}
		free(buf);
		free(src);
		free(dest);
		free(temp_path);
		free(src_dir);
		free(working_dir);
		// close used directory
		closedir(dp);
	}

	else if (cat==1) // files concatented
	{
		// for each file open, copy, close it
		stat(files->name,&st);

		//		if (src==NULL) // src not already allocated
		//			src = (char *) malloc(sizeof(char)*strlen(files->name)+1);

		// override not allowed
		if (override==0 && !overridden(dest))
			exit(0);

		if ( !alpha_num(dest,&c,0) || (dest_fd=open (dest, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
		{
			fprintf(stderr, "Can't create dest file %s\n",dest);
			exit(1);
		}
		while (files != NULL)
		{
			stat(files->name,&st);
			src = (char *) malloc(sizeof(char)*strlen(files->name)+1);
			strcpy(src,files->name);

			if ( (source_fd=open (src, O_RDONLY)) == -1)
			{
				fprintf(stderr, "Can't open source file %s\n",src);
				exit(1);
			}

			while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
				write( dest_fd, buf, rd );

			// increment counter of file sizes that can be used for verify next
			count_concat+=st.st_size;

			close(source_fd);
			files=files->next;
			free(src);
		}

		// verify
		if (verify==1)
		{
			stat(dest,&st_dest);
			if (count_concat == st_dest.st_size) // problems with copy -> if files of different sizes
				fprintf(stdout,"Source files succesfully copied\n");
		}


		free(buf);
		//free(src);
		free(dest);
		close(dest_fd);	
	}

	else // file copy file-file
	{	
		stat(src,&st);

		// override not allowed
		if (override==0 && !overridden(dest))
			exit(0);

		if ( !alpha_num(dest,&c,0) || (dest_fd=open (dest, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
		{
			fprintf(stderr, "Can't open dest file %s\n",dest);
			exit(1);
		}

		/*rd=read(source_fd, buf, st.st_size+1);
	   	printf("read %d bytes of size %ld %u \n", rd,st.st_size, st.st_mode);

	   	rd=write(dest_fd, buf, rd);
	   	printf("wrote %d bytes\n", rd);
		 */

		while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
			write( dest_fd, buf, rd );

		// verify
		if (verify==1)
		{
			stat(dest,&st_dest);
			if (st.st_size == st_dest.st_size) // problems with copy -> if files of different sizes
				fprintf(stdout,"Source file %s succesfully copied\n",src);
		}

		free(buf);
		free(src);
		free(dest);
		close(source_fd);
		close(dest_fd);
	}

}

// ask user permission to create file
int confirmation(char *pth)
{
	char *answ;

	while(1)
	{
		printf ("Are you sure to create file %s?[Yes or No]\n", pth);

		answ=get_line();
		if (strncmp(answ,"yes",3)==0 || strncmp(answ,"YES",3)==0 || strncmp(answ,"Yes",3)==0 )
			return 1;
		else if ( strncmp(answ,"y",1)==0 || strncmp(answ,"Y",1)==0 )
			return 1;
		else	// NO
		{
			if (strncmp(answ,"no",2)==0 || strncmp(answ,"NO",2)==0 || strncmp(answ,"No",2)==0)
				return 0;
			else if ( strncmp(answ,"n",1)==0 || strncmp(answ,"N",1)==0 )
				return 0;
			else 
				fprintf(stderr, "Type Yes or No !\n");
		}
	}
}

// basic copy of file to file
// used for recursive dir copy
void basic_copy(char *src, char *dest)
{
	char *buf;
	char *working_dir;
	int source_fd, dest_fd;
	int rd;
	struct stat st, st_dest;

	// allocate buffer for copying
	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);

	stat(src,&st);

	// ask confirm before creating file
	if (confirm==1 && !confirmation(dest))
		return;
	
	// read-only file
	if (overread==0 && rd_only(dest) )
	{
		printf ("Read-Only file %s\n",dest);
		if (!overridden(dest))
			return;
	}	
	// override not allowed
	else if (override==0 && !overridden(dest))
		return;

	if ((dest_fd=open (dest, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
	{
		fprintf(stderr, "Can't open dest file %s\n",dest);
		return;
	}

	while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
		write( dest_fd, buf, rd );

	// verify
	if (verify==1)
	{
		stat(dest,&st_dest);
		if (st.st_size == st_dest.st_size) // problems with copy -> if files of different sizes
			fprintf(stdout,"Source file %s succesfully copied\n",src);
	}

	free(buf);
	free(src);
	free(dest);
	close(source_fd);
	close(dest_fd);
}

/*
 * recursive function that enter recursively
 * directories, copy files and subdir to dest directory 
 */
void recur_dir_copy(char *src_path, char *dest_path)
{
	DIR *dp;
	struct dirent *ep;
	char *temp_src, *temp_dest;
	param *dir_list=NULL;

	temp_src = (char *) malloc( sizeof(char)*MAXPATH+1 );
	temp_dest = (char *) malloc( sizeof(char)*MAXPATH+1 );

	dp = opendir(src_path);

	if (dp != NULL) {
		while ( (ep = readdir(dp)) )
		{
			strcpy(temp_src,src_path);
			strcpy(temp_dest,dest_path);

			//if the current dir doesn't finish with '/',
			//I should add it before appending the currente resource name
			if ( src_path[strlen(src_path)-1] != '/')
				strcat(temp_src, "/");				
			strcat(temp_src, ep->d_name);
			//create dest path
			if ( dest_path[strlen(dest_path)-1] != '/')
				strcat(temp_dest, "/");				
			strcat(temp_dest, ep->d_name);

			//ep->type: 4 dir, 8 file
			if ( ep->d_type==4)	// is a directory get into recursively
			{
				if (subdir!=0 && strcmp(ep->d_name, ".") != 0  && strcmp(ep->d_name, "..")!= 0 )
				{
					// create dest dir
					dir_list=parse_options(temp_dest,0,0);
					md(dir_list);

					printf("Dir %s will create dir %s\n", temp_src, temp_dest);
					recur_dir_copy(temp_src,temp_dest);
				}
			}

			else // is a file
			{
				if (no_file==0 && strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..")!= 0 )
				{
					if (no_copy!=0) // only print
						printf("source file %s would becopied in %s\n", temp_src, temp_dest);
					else // copy file
					{
						basic_copy(temp_src,temp_dest);
						if (show_name!=0) // display names after copy
							printf("source file %s copied in %s\n", temp_src, temp_dest);
					}
				}
			}
		}
		closedir(dp);
	}
	free(temp_src);
	free(temp_dest);
	return;
}

// copy dir recursively
void xcp(param *list)
{
	char *buf, *temp_path, c;
	char *src, *dest;
	char *working_dir, *src_dir;
	int source_fd, dest_fd;
	int rd, stat_res;
	param *dir_list=NULL;
	struct stat st, st_dest;
	struct dirent *ep;
	DIR *dp;
	off_t count_concat=0;

	// override=0; !!!
	// global
	/*
	int verify=0, override=1;
	int confirm=0, subdir=0, empty=0;
	int show_name=0, no_copy=0, no_file=0, overread=0;
	 */

	int flag=0, dir=0, dest_dir=0;

	if(list==NULL)
	{
		fprintf(stderr, "copy: missing file operand\n");
		fprintf(stderr, "Try \'help copy\' for more information\n");
		exit(1);
	}

	// scroll list of parameters
	while (list!=NULL)
	{
		printf("---%s---\n", list->name);
		if ( list->type==0 ) //path
		{
			// take info about file / dir
			stat_res=stat(list->name,&st);

			if (flag==0) // source
			{
				flag=1;

				if ( S_ISDIR(st.st_mode) && stat_res>=0 ) // source is a directory
				{
					dir=1;
					if (opendir( list->name ) == NULL)
						err();
					src_dir = (char *) malloc(sizeof(char)*strlen(list->name)+1);
					strcpy(src_dir,list->name);
				}
				else if ( S_ISREG(st.st_mode) && stat_res>=0 ) // source is a file
				{
					src = (char *) malloc(sizeof(char)*strlen(list->name)+1);
					strcpy(src,list->name);

					// try to open source file
					if ( (source_fd=open (src, O_RDONLY)) == -1)
					{
						fprintf(stderr, "Can't open source file %s\n",src);
						exit(1);
					}
				}
				else 
					err();

			}
			else if (flag==1) // destination
			{
				flag=2;

				if ( S_ISDIR(st.st_mode) && stat_res>=0) // dest is a directory
				{
					if (opendir( list->name ) == NULL)
						err();

					dest = (char *) malloc(sizeof(char)*strlen(list->name)+1);
					strcpy(dest,list->name);
					dest_dir=1;
					printf("dest dir existing\n");
				}	
				else // dest is a file or a dir to be created
				{
					if ( strncmp(list->name+strlen(list->name)-1, "/", 1)==0 ) // dir to be created
					{
						dest = (char *) malloc(sizeof(char)*strlen(list->name)+1);
						strcpy(dest,list->name);
						// create dir
						dir_list=parse_options(list->name,&dest_fd,0); // dest_fd just to avoid warning, useless
						md(dir_list);

						dest_dir=1;
						printf("dest dir 1\n");
					}

					else  // file
					{
						if (dir==1) // dest file is not valid if source is a dir
						{
							fprintf(stderr, "Error, source is a directory and destination is a file\n");
							exit(1);
						}
						else
						{
							dest = (char *) malloc(sizeof(char)*strlen(list->name)+1);
							strcpy(dest,list->name);
						}
					}
				}

			}
			// flag==2 -- too much parameters
			else
				err();	
		}

		else if ( list->type==1 ) //opt TODO
		{	
			if ( strcasecmp(list->name,"\\V") )
				verify=1;
			else if ( strncasecmp(list->name,"\\-Y",3) )
				override=0;
			else if ( strcasecmp(list->name,"\\Y") ){
				override=1;
				printf(" here\n");
			}
			else if ( strncasecmp(list->name,"\\P",2) )
				confirm=1;
			else if ( strncasecmp(list->name,"\\S",2) )
				subdir=1;
			else if ( strncasecmp(list->name,"\\Q",2) )
				show_name=0;
			else if ( strncasecmp(list->name,"\\F",2) )
				show_name=1;
			else if ( strncasecmp(list->name,"\\L",2) )
				no_copy=1;
			else if ( strncasecmp(list->name,"\\R",2) )
				overread=1;
			else if ( strncasecmp(list->name,"\\T",2) )
				no_file=1;
			else if ( strncasecmp(list->name,"\\E",2) )
			{
				if (subdir!=0 || no_file!=0)  // also \t or \s should be found
					empty=1;
				else
				{
					fprintf(stderr, "%s should be set with \\S or \\T\n",list->name );
					err();
				}
			}
			else
			{
				fprintf(stderr, "%s - unknown option\n",list->name );
				exit(1);
			}
		}
		else
			err();

		list= list->next;
	} // end of while - parsing line of parameters

	// check that enough resources were specified
	if (flag<2) 
		err();

	// allocate buffer for copying
	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);

	// dest is a dir
	if (dest_dir==1)
	{
		if (dir==1) // dir-dir copy recursive
		{
			printf("dest dir 2\n");

			// calling recursive function
			recur_dir_copy(src_dir,dest); // verify,override,confirm,subdir,empty,show_name,no_copy,no_file,overread);
		}
		else // file-dir copy
		{
			stat(src,&st);		
			// create file name
			temp_path = (char *) malloc( sizeof(char)*MAXPATH+1);

			// create file in dest directory
			strcpy(temp_path,dest);

			//if the current dir doesn't finish with '/',
			//I should add it before appending the currente resource name
			if ( dest[strlen(dest)-1] != '/')
				strcat(temp_path, "/");				
			strcat(temp_path, src);

			if (  (dest_fd=open (temp_path, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
			{
				fprintf(stderr, "Can't create dest file %s\n",temp_path);
				exit(1);
			}

			while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
				write( dest_fd, buf, rd );

			close(source_fd);
			close(dest_fd);

			fprintf(stdout,"File %s succesfully copied\n", temp_path);

			free(buf);
			free(src);
			free(dest);
			free(temp_path);
		}
	}

	else // file copy file-file
	{	
		stat(src,&st);

		// override not allowed
		if (override==0 && !overridden(dest))
			exit(0);

		if ( !alpha_num(dest,&c,0) || (dest_fd=open (dest, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
		{
			fprintf(stderr, "Can't open dest file %s\n",dest);
			exit(1);
		}

		while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
			write( dest_fd, buf, rd );

		// verify
		if (verify==1)
		{
			stat(dest,&st_dest);
			if (st.st_size == st_dest.st_size) // problems with copy -> if files of different sizes
				fprintf(stdout,"Source file %s succesfully copied\n",src);
		}

		free(buf);
		free(src);
		free(dest);
		close(source_fd);
		close(dest_fd);
	}

}
