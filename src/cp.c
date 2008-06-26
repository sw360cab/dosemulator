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

// PROTOTYPES

// print error message, when files involved have problems
void err ();
// check if the source file is a concatenation --> contains '+'
int concat (char *);
// create the list of files in case of a concatetanation
param* create_source(char *);
//ask confirmation to override existing dest -  0-> dir 1->file
int req_overridden(char *, int);
// check if destination file exists
// in case ask user what to do  -  0-> dir 1->file
int overridden(char *, int);
// copy files
void cp(param *);
// ask user permission to create dest -  0-> dir 1->file
int create_confirmation(char *, int);
// basic copy of file to file
// used for recursive dir copy
void basic_copy(char *, char *);
// basic copy of dir, if dir exist erase it
// otherwise only create it
void basic_dir_copy(char *, char *);
/*
 * recursive function that enter recursively
 * directories, copy files and subdir to dest directory 
 */
int recur_dir_copy(char *src_path, char *dest_path);
// ask user if path is intended as dir -> true or as a file -> false
int is_dest_dir(char *);
// copy dir recursively
void xcp(param *);
/*
 * fake write on device floppy --> floppy.out
 * !!! IMPORTANT must be ROOT to compile-execute this correctly
 */
void disk_copy(param *);
// from rm.c
extern int rd_only(char *);


// global option flag
// set all to 0 !!!
int verify=0, override=0;
int confirm=0, subdir=1, empty=0;
int show_name=0, no_copy=0, no_file=0, overread=0;

// print error message, when files involved have problems
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

		//fprintf(stdout,"%s to be parsed\n", p->name);
		p->type = 0;
		p->next=NULL;

		insert_e(&pt,p);

		files+=count+1;
		count=0;
	}while ( strncmp( files, "\0",1)!=0 );
	return pt;	
}

//ask confirmation to override existing dest -  0-> dir 1->file
int req_overridden(char *pth, int file)
{
	char *answ;

	while(1)
	{

		if (file==1) // file
			fprintf(stdout,"File %s already exists, Do you want to overwrite it? [Yes or No or All]\n", pth);
		else // dir
			fprintf(stdout,"Direcory %s already exists, Do you want to overwrite it? [Yes or No or All]\n", pth);

		answ=get_line();
		if (strcasecmp(answ,"yes")==0 || strcasecmp(answ,"y")==0 || strcmp(answ,"Yes")==0 )
			return 1;
		else	
		{	// NO
			if ( strcasecmp(answ,"no")==0 || strcasecmp(answ,"n")==0 || strcmp(answ,"No")==0 )
				return 0;
			// ALL - don't ask again
			else if ( strcasecmp(answ,"all")==0 || strcasecmp(answ,"a")==0 || strcmp(answ,"All")==0 )
			{
				override = 1;
				return 1;
			}
			else 
				fprintf(stderr, "Type Yes or No or All !\n");
		}

	}
}

// check if destination file exists
// in case ask user what to do -  0-> dir 1->file
int overridden(char *dest, int file)
{
	int dest_fd;

	if ( (dest_fd=open (dest, O_RDONLY)) > 0)  // dest file exist
	{
		if ( req_overridden(dest,file) == 0 )  // ask user what to do
		{
			close(dest_fd);
			return FALSE;
		}
		close(dest_fd);
		return TRUE;
	}
	return TRUE;
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
	int flag=0, cat=0, dir=0, count=0;


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
		// fprintf(stdout,"---%s---\n", list->name);
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

		else if ( list->type==1 ) // options
		{	
			// check options
			//if ( strcmp(list->name,"\A") ||  strcmp(list->name,"\B") )
			if ( strcasecmp(list->name,"\\V")==0 )
				verify=1;
			else if ( strcasecmp(list->name,"\\-Y")==0 )
				override=0;
			else if ( strcasecmp(list->name,"\\Y")==0 )
				override=1;
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
					if (override==0 && !overridden(temp_path,1))
						continue; // go to next file

					if (  (dest_fd=open (temp_path, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
					{
						fprintf(stderr, "Can't create dest file %s\n",temp_path);
						exit(1);
					}

					while ((rd = read( source_fd, buf,BUF_MAX+1)) > 0 )
						write( dest_fd, buf, rd );
					count++;
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
			fprintf(stdout,"\n%d files copied\n\n", count);
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
		if (override==0 && !overridden(dest,1))
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
			count++;

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
		fprintf(stdout,"\n%d files copied\n\n",count);

		free(buf);
		//free(src);
		free(dest);
		close(dest_fd);	
	}

	else // file copy file-file
	{	
		stat(src,&st);

		// override not allowed
		if (override==0 && !overridden(dest,1))
			exit(0);

		if ( !alpha_num(dest,&c,0) || (dest_fd=open (dest, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
		{
			fprintf(stderr, "Can't open dest file %s\n",dest);
			exit(1);
		}

		/*rd=read(source_fd, buf, st.st_size+1);
	   	fprintf(stdout,"read %d bytes of size %ld %u \n", rd,st.st_size, st.st_mode);

	   	rd=write(dest_fd, buf, rd);
	   	fprintf(stdout,"wrote %d bytes\n", rd);
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
		fprintf(stdout,"\n1 file copied\n\n");

		free(buf);
		free(src);
		free(dest);
		close(source_fd);
		close(dest_fd);
	}

}

// ask user permission to create dest
int create_confirmation(char *pth, int file)
{
	char *answ;

	while(1)
	{
		if (file==1) // file
			fprintf(stdout,"Are you sure to create file %s?[Yes or No or All]\n", pth);
		else // dir
			fprintf(stdout,"Are you sure to create directory %s?[Yes or No or All]\n", pth);
					
		answ=get_line();
		if (strcasecmp(answ,"yes")==0 || strcasecmp(answ,"y")==0 || strcmp(answ,"Yes")==0 )
			return 1;
		else	
		{	// NO
			if ( strcasecmp(answ,"no")==0 || strcasecmp(answ,"n")==0 || strcmp(answ,"No")==0 )
				return 0;
			// ALL - don't ask again
			else if ( strcasecmp(answ,"all")==0 || strcasecmp(answ,"a")==0 || strcmp(answ,"All")==0 )
			{
				confirm = 0;
				return 1;
			}
			else 
				fprintf(stderr, "Type Yes or No or All !\n");
		}
	}
}

// basic copy of file to file
// used for recursive dir copy
void basic_copy(char *src, char *dest)
{
	char *buf;
	int source_fd, dest_fd;
	int rd;
	struct stat st, st_dest;

	// allocate buffer for copying
	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);

	stat(src,&st);
	if ( (source_fd=open (src, O_RDONLY)) == -1)
	{
		fprintf(stderr, "Can't open source file %s\n",src);
		return;
	}

	// ask confirm before creating file
	if (confirm==1 && !create_confirmation(dest,1))
		return;

	// read-only file
	if (overread==0 && rd_only(dest) )  // overread flag -- ask before overwriting
	{									//					read-only file
		fprintf(stdout,"Read-Only file %s\n",dest);
		if (!overridden(dest,1))
			return;
	}	
	// override not allowed
	else if (override==0 && !overridden(dest,1))
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
		else 
			fprintf(stdout,"Pain\n");
	}

	free(buf);
	close(source_fd);
	close(dest_fd);
}


// basic copy of dir, if dir exist erase it
// otherwise only create it
void basic_dir_copy(char *src, char *dest)
{
	struct stat st, st_dest;
	int stat_res;
	param *p;

	//md(dir_list);
	fprintf(stdout,"Dir %s will create dir %s\n", src, dest);

	stat(src,&st);
	stat_res=stat(dest,&st_dest);

	if (no_copy!=0) // no_copy flag -- only print
		fprintf(stdout,"source directory %s would be created as %s\n", src, dest);					
	else 
	{
		if ( S_ISDIR(st_dest.st_mode) && stat_res>=0 ) // directory already exist -> erase before creting
		{
			// ask confirm before creating file
			if (confirm==1 && !create_confirmation(dest,0))
				return;

			// override not allowed
			else if (override==0 && !overridden(dest,0))
				return;

			// erase directory
			p=new_elem();
			p->name = (char *) malloc(sizeof(char)*strlen(dest)+1);
			strcpy(p->name, dest);
			p->type = 0;
			p->next=NULL;

			deltree(p);
			free(p);
		}

		//md(dir_list);
		if (mkdir(dest, st.st_mode) < 0 )
		{
			fprintf (stdout,"Unable to create directory or you don't have permission to do so\n");
			exit(1);
		}
		if (show_name!=0) // display names after copy
			fprintf(stdout,"source directory %s created as %s\n", src, dest);		
	}
}

/*
 * recursive function that enter recursively
 * directories, copy files and subdir to dest directory 
 */
int recur_dir_copy(char *src_path, char *dest_path)
{
	DIR *dp;
	struct dirent *ep;
	struct stat st;
	char *temp_src, *temp_dest;
	int dest_fd, count=0;

	temp_src = (char *) malloc( sizeof(char)*MAXPATH+1 );
	temp_dest = (char *) malloc( sizeof(char)*MAXPATH+1 );

	dp = opendir(src_path);

	//fprintf(stdout,"source is %s - dest is %s\n", src_path, dest_path);

	if (dp != NULL) 
	{
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
				{ // subdir flag -- enter in subdirectories
					if(!empty_dir(temp_src) || empty==1)  // empty flag -- enter also if dir is empty
					{
						/*if (mkdir(temp_dest, st.st_mode) < 0 )
						{
							fprintf (stdout,"Unable to create directory or you don't have permission to do so\n");
							exit(1);
						}*/
						
						basic_dir_copy(temp_src,temp_dest);
						// recursive call
						count+=recur_dir_copy(temp_src,temp_dest);
					}
					else
						fprintf (stderr,"Skipping %s empty directory\n", temp_dest);

				}
			}

			else // is a file
			{
				// no_file flag -- create only dir tree
				if (no_file==0 && strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..")!= 0 )
				{
					if (no_copy!=0) // no_copy flag -- only print
						fprintf(stdout,"source file %s would be copied in %s\n", temp_src, temp_dest);
					else // copy file
					{
						basic_copy(temp_src,temp_dest);
						count++;
						if (show_name!=0) // display names after copy
							fprintf(stdout,"source file %s copied in %s\n", temp_src, temp_dest);
					}
				}
			}
		}
		closedir(dp);
	}
	free(temp_src);
	free(temp_dest);
	return count;
}

// ask user if path is intended as dir -> true or as a file -> false
int is_dest_dir(char *pth)
{
	char *answ;

	while(1)
	{
		fprintf(stdout,"The destination %s is a file[F] or a directory[D]?\n", pth);

		answ=get_line();
		if (strcasecmp(answ,"dir")==0 || strcasecmp(answ,"d")==0 || strcmp(answ,"Dir")==0 )
			return TRUE;
		else	
		{	// file
			if ( strcasecmp(answ,"file")==0 || strcasecmp(answ,"F")==0 || strcmp(answ,"File")==0 )
				return FALSE;
			else 
				fprintf(stderr, "Type F or D!\n");
		}
	}
}

// copy dir recursively
void xcp(param *list)
{
	char *buf, *temp_path, c;
	char *src, *dest, *temp_dest;
	char *src_dir;
	int source_fd, dest_fd;
	int rd, stat_res;
	struct stat st, st_dest;
	// param *dir_list=NULL;

	int flag=0, dir=0, dest_dir=0, count=0;

	if(list==NULL)
	{
		fprintf(stderr, "xcopy: missing file operand\n");
		fprintf(stderr, "Try \'help copy\' for more information\n");
		exit(1);
	}

	// scroll list of parameters
	while (list!=NULL)
	{
		fprintf(stdout,"---%s---\n", list->name);
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

				if ( S_ISDIR(st.st_mode) && stat_res>=0 ) // dest is an existing directory 
				{
					if (opendir( list->name ) == NULL)
						err();
					dest = (char *) malloc(sizeof(char)*strlen(list->name)+1);
					strcpy(dest,list->name);
					
					if (dir==1) // source is a dir open dest dir
					{
						dest_dir=1;
						fprintf(stdout,"dest dir existing\n");
					}
					else //source is a file - enter in dest directory
					{
						chdir(dest);
						fprintf(stdout,"File %s would be copied in directory %s\n",src,dest);
						strcpy(dest,src);
					} 
				}	
				
				else if ( S_ISREG(st.st_mode) && stat_res>=0 )// dest is a file existing
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

				//  dest is a dir
				else if ( is_dest_dir(list->name) )
				{
					dest = (char *) malloc(sizeof(char)*strlen(list->name)+1);
					strcpy(dest,list->name);
					// create dir
					//dir_list=parse_options(list->name,&dest_fd,0); // dest_fd just to avoid warning, useless
					//md(dir_list)
					if (mkdir(dest, st.st_mode) < 0 )
					{
						fprintf (stdout,"Unable to create directory or you don't have permission to do so\n");
						exit(1);
					}

					if (dir==1) // source is a dir and dest is a dir to be created
					{
						dest_dir=1;
					}
					else //source is a file - enter in dest directory
					{
						chdir(dest);
						//fprintf(stdout,"File %s would be copied into created directory %s\n",src,dest);
						fprintf(stdout,"chdir to %s\n",dest);				
						dest = (char *) realloc(dest, sizeof(char)*strlen(src)+1);
						strcpy(dest,src);
						fprintf(stdout,"new dir is %s\n",dest);
					}
				}

				else // dest is a file
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
			{
				fprintf(stderr,"Too much parameteres\n");
				err();
			}
		}

		else if ( list->type==1 )  // options
		{	
			// check options
			if ( strcasecmp(list->name,"\\V")==0 )  // verify written files
				verify=1;
			else if ( strcasecmp(list->name,"\\-Y")==0 )	// disallow overwriting of existing files without asking
				override=0;
			else if ( strcasecmp(list->name,"\\Y")==0 )		// allow overwriting of existing files without asking
				override=1;
			else if ( strcasecmp(list->name,"\\P")==0 )		// request confirmation before creating each dest file
				confirm=1;
			else if ( strcasecmp(list->name,"\\S")==0 )		// copy dir and subdir
				subdir=1;
			else if ( strcasecmp(list->name,"\\Q")==0 )		// do not show names while copying
				show_name=0;
			else if ( strcasecmp(list->name,"\\F")==0 )		// show names while copying
				show_name=1;
			else if ( strcasecmp(list->name,"\\L")==0 )		// do not copy only show files that would be copied
				no_copy=1;
			else if ( strcasecmp(list->name,"\\R")==0 )		// overwrite read-only files
				overread=1;
			else if ( strcasecmp(list->name,"\\T")==0 )		// only crete directory tree structure - do not copy inner files
				no_file=1;
			else if ( strcasecmp(list->name,"\\E")==0 )		// do not avoid copying empty dir
			{
				if (subdir!=0 || no_file!=0)  // also \S or \T should be found
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
	{
		fprintf(stderr,"source or destination file missing\n");
		err();
	}

	// allocate buffer for copying
	buf = (char *) malloc(sizeof(char)*BUF_MAX+1);

	// dest is a dir
	if (dest_dir==1)
	{
		if (dir==1) // dir-dir copy recursive
		{
			fprintf(stdout,"dest dir 2\n");

			// calling recursive function
			count=recur_dir_copy(src_dir,dest);
			fprintf(stdout,"\n%d files copied\n\n", count);
		}
		else // file-dir copy  - open dir / create path-file / copy file in directory
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

		// ask confirm before creating file
		if (confirm==1 && !create_confirmation(dest,1))
			exit(0);

		// read-only file
		if (overread==0 && rd_only(dest) )  // overread flag -- ask before overwriting
		{									//					read-only file
			fprintf(stdout,"Read-Only file %s\n",dest);
			if (!overridden(dest,1))
				exit(0);
		}	
		// override not allowed
		else if (override==0 && !overridden(dest,1))
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

/*
 * fake write on device floppy --> floppy.out
 * !!! IMPORTANT must be ROOT to compile-execute this correctly
 */
void disk_copy(param* list)
{
	char path[10], *buf, *src;
	int source_fd, floppy_fd;
	int written_bytes=0;
	int rd, stat_res;
	struct stat st;

	if(list==NULL)
	{
		fprintf(stderr, "diskcopy: missing file operand\n");
		fprintf(stderr, "Try \'help copy\' for more information\n");
		exit(1);
	}

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

