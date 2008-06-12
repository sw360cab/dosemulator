#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "parse.h"

void cls(param **parameters){
	
	param *iterator = (*parameters);
	short flag= FALSE;
	
		
	while(iterator!=NULL){
		
		
		if(strlen(iterator->name)>0)
			flag=TRUE;
		
		iterator=iterator->next;
	}
	
	if(flag==TRUE){
		
	
		fprintf(stderr,"Wrong command syntax\n");
		
	}
		
	else{
		
		system("clear");
	}
	
	
}
