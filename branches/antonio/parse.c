#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

int jolly_char(char *line)
{
	if (strncmp( line, "/",1)==0)
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
	
	do
	{
		while ( strncmp( opt+count, " ",1)!=0 && strncmp( opt+count,"\0",1)!=0 )
			count++ ;
		end=count;
		p=new_elem();
		
		p->name = (char *) malloc(sizeof(char)* (end) );
		strncpy(p->name, opt, end);
		printf("%s to be parsed\n", p->name);
		if (jolly_char(opt) )
			p->type = 1;
		else 
			p->type = 0;
		p->next=NULL;
		
		insert_e(&pt,p);
			
		opt+=count+1;
		start=end+1;
		count=0;
	}while ( strncmp( opt, "\0",1)!=0 );
	
	return pt;	
}

void parse_line(char **comm, char ** opt, char * line)
{	
	int count=0;
	
	while ( strncmp( line+count, " ",1)!=0 )
		count++ ;
	
	strncpy(*comm,line,count); // TODO Change function with strnchr or similar
	strncpy(*opt,line+count+1, strlen(line) - count +1);
}
