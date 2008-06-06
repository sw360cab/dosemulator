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
	if (strcmp( line, ">")==0 || strcmp( line, ">>")==0) // redirection
		return 2;
	else if (strncmp( line, "\\",1)==0)
		return 1;
	else if (strncmp( line, "+",1)==0)
		return 1;
	else if (strncmp( line, "-",1)==0)
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

param* parse_options(char *opt)
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
			p=new_elem();
			
			p->name = (char *) malloc(sizeof(char)* (end) );
			strncpy(p->name, opt, end);
			printf("%s to be parsed\n", p->name);
			
			p->type = jolly_char(opt);
			p->next=NULL;
			
			insert_e(&pt,p);
				
			opt+=count+1;
			start=end+1;
			count=0;
		}while ( strncmp( opt, "\0",1)!=0 );
	}
	
	return pt;	
}

void parse_line(char **comm, char ** opt, char * line)
{	
	int count=0;
	
	while ( strncmp( line+count, " ",1)!=0 && strncmp( line+count, "\0",1)!=0 )
		count++;
	
	strncpy(*comm,line,count); // TODO Change function with strnchr or similar
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
