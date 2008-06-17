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
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

int jolly_char(char *line)
{
	if (strncmp( line, "\\", 1)==0)
		return 1;
	else if (strncmp( line, "+", 1)==0)
		return 1;
	else if (strncmp( line, "-", 1)==0)
		return 1;
	else 
		return 0;
}

void insert_e(param **p_list, param *p)
{
	param *temp;
	
	temp=(*p_list);
	
	if ((*p_list)==NULL)
		(*p_list)=p;
	else
		{ 
		// find last 
		while (temp->next!=NULL)
			temp=temp->next;
			
		temp->next=p;
		}
}

param * new_elem()
{
	param *p;
	if ((p=(param *)malloc(sizeof(param)))==NULL)
	  { printf("Error  : Memory allocation error\n");
	    exit(1);
	  }
	return p;
}

/*
 * '-' and '_' are allowed for paths
 * '.' and '/' supposed allowed for files
 */
int under_s(char ch, int dir)
{
	if (ch=='-' || ch =='_')
		return TRUE;
	else
	{
		if (dir==0) // '.' is allowed
			{
			if (ch=='.' || ch =='/')
					return TRUE;
			}
		else
			return FALSE;
	}
	return FALSE;
}

/*
 * check for forbidden characters
 * 
 * dir is 1 --> name to check is a directory name
 * dir is 0 --> name to check is a file name --> '.' is a valid char
 */
int alpha_num(char *path, char *ch, int dir)
{
	int i,count=strlen(path);

	// if name is alphaneumeric or contains '-' or '_' or '.' (in case of file name is OK
	for(i=0;i<strlen(path) && ( isalnum(path[i]) || under_s(path[i],dir) ); i++)
	{
		count--;
		//printf ("cnt = %d-%s-%d-%d-\n",count,path, isalnum(path[i]),k);
	}

	if (count==0)
		return TRUE;

	else
	{
		strncpy(ch,path+i,1);
		return FALSE;
	}
}

// function in charge of handling redirection
// return file pointer in case of success
int redirector(char *str_file, int str_len) {
	int count=0, append=0; // flag for appending or opening
	char *file_name;
	char c;
	mode_t mode;
	int fd;
	
	file_name= (char*)malloc(sizeof(char)*str_len);
	
	if (strncmp(str_file,">>",2)== 0) // appeding mode
	{
		append=1;
		str_file+=2;
	}
	else
		str_file+=1;
	
	// avoid blank spaces before file name
	while ( strncmp( str_file," ",1)==0 )
		str_file++;
	
	//printf("okkk ---%s--%d---\n", str_file,str_len);
		
	// check that only a parameter is specified as file name
	// and that file name is valid
	while ( strncmp( str_file+count,"\0",1)!=0 )
	{
		if ( strncmp( str_file+count," ",1)==0 ) // blank spaces in the middle, WRONG !!
		{
			fprintf(stderr,"Only one file name allowed as parameter\n");
			exit(1);
		}
		
		if (!alpha_num(str_file,&c,0)) // unexpected char
		{
			fprintf (stderr,"Invalid file name, unexpected char %c\n", c);
			exit(1);
		}
		count ++;
	}
	
	// set file name
	strcpy(file_name, str_file);
	//printf("done --%s-- \n", file_name);
	
	// retrieve file descriptor if everything was 0k
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	if(append==1) // append mode
	{
		//open in append mode
		if ( (fd=open (file_name, O_APPEND | O_WRONLY, mode)) == -1)
		{
			fprintf(stderr, "Can't open file %s, file will be created\n",file_name);
			if ( (fd=open (file_name, O_CREAT | O_WRONLY | O_TRUNC, mode)) == -1)
			{
				fprintf(stderr, "Can't create file %s\n",file_name);
				exit(1);
			}
		}
	}
	else  // write mode
	{
		//open in write mode
		if ( (fd=open (file_name, O_CREAT | O_WRONLY | O_TRUNC, mode)) == -1)
		{
			fprintf(stderr, "Can't create file %s\n",file_name);
			exit(1);
		}
	}
	
	//printf("end --%d-- \n", fd);
	return fd;
}

char* pipe_string(char *str)
{
	char *line;
	
	line = (char *) malloc(sizeof(char)* strlen(str) );
	// move until pipe
	line = strchr(str,'|');
	// move over pipe
	line++;
	
	// check if blank space or not after pipe
	if (strncmp(line," ",1)==0)
		line++;

	return line;
}

param* parse_options(char *opt, int *file_d, int *pipe)
{	
	int start, end, count=0;
	param *p;
	param *pt=NULL;
	
	start=count;
	
	// return immediately if no options are present
	if (strlen(opt)!=0)
	{
		do
		{
			while ( strncmp( opt+count, " ",1)!=0 && strncmp( opt+count,"\0",1)!=0 )
				count++ ;
			end=count;
			
			if ( strncmp(opt,">",1)== 0) // redirection was asked
				{
				*file_d = redirector(opt,strlen(opt));
				
				while ( strncmp( opt,"\0",1)!=0 )  // move to end of line in order to exit do-while
					opt++;
				}
			if ( strncmp(opt,"|",1)== 0) // pipe command
				{
				*pipe=1;
				while ( strncmp( opt,"\0",1)!=0 )  // move to end of line in order to exit do-while
					opt++;
				}
			else
			{
				p=new_elem();

				p->name = (char *) malloc(sizeof(char)* (end) );
				strncpy(p->name, opt, end);
				//printf("%s to be parsed\n", p->name);

				p->type = jolly_char(opt);
				p->next=NULL;

				insert_e(&pt,p);

				opt+=count+1;
				start=end+1;
				count=0;
			}
		}while ( strncmp( opt, "\0",1)!=0 );
	}
	
	return pt;	
}

void parse_line(char **comm, char ** opt, char * line)
{	
	int count=0;
	
	while ( strncmp( line+count, " ",1)!=0 && strncmp( line+count, "\0",1)!=0 )
		count++;
	
	strncpy(*comm,line,count);
	
	if(strncmp( line+count, "\0",1)!=0)
		strncpy(*opt,line+count+1, strlen(line) - count +1);
}

char *salloc (unsigned size)
{
    char *ret;
    ret = (char *) malloc (size+1);
        
    return ret;
}

char *srealloc(char *str, unsigned u){
	
	char *ret;
	unsigned int length = u + strlen(str);
	
	
	ret = (char *) realloc(str,length);
	    
	return ret;	
}

char *get_line(){
	
	char *line;
	char *cp;
	
	unsigned size;
	unsigned count;
	int c;
	
	size = N;
	line=salloc(size);
	cp=line;
	for(count=0;(c=getchar())!= '\n'; count++){
		
		if(c==EOF)
			return NULL;
		if(count>=(size-1))
		{
			line=srealloc(line,size +=N);
			cp=line+count;
		}
		*cp++=c;
	}
	*cp ='\0';
	count++;
	return srealloc(line,count);
}
