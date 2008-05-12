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

int main(){
	char *string;
	
	string = get_line();
	printf("%s\n",stringoàèo);
	return 0;
}
