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
#include <string.h>
#include <stdlib.h>
#define N 20

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

char *srealloc_new(char *str, unsigned u){

		char *ret;
		unsigned int length = u;
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
/*
int main(){
	char *string;
	
	string = get_line();
	printf("%s\n",string);
	return 0;
}*/
