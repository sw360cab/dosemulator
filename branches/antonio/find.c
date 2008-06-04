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
//TODO more than one file in which to search
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

//TODO how to define a good string length ?-> see nw.c in docs/readFunctions.zip 
#define STRING_LENGTH 1024


short v_option; //show not matched
short c_option; //show only the number of matches
short n_option; //show also line number: it has effciency only if c=false
short i_options; //ignore uppercase

void my_find(char *string, char *src) {

	int row_counter=0, nr_matches=0, i=0, i2=0;
	FILE *fp;
	char *buf, *output, *buf2;
	
	short matched=FALSE;

	if ( (fp = fopen(src, "r")) == NULL) {
		fprintf(stderr,"FIND: cannot access : %s: No such file or directory\n",
		src);
		//exit(1);
		return;
	}

	//if argument 1 doesn't begin with "" quotes,error. it seems impossible to emulate


	//TODO src uppercase
	if (c_option == FALSE)
		printf("\n----------%s\n", src);

	buf = (char *) malloc((unsigned int)STRING_LENGTH);
	if (i_options==TRUE)
		buf2=(char *) malloc((unsigned int)strlen(string));

	while (fgets(buf, (int)STRING_LENGTH, fp) != NULL) {

		row_counter++;

		if (i_options==TRUE && strlen(buf)>=strlen(string) ) {

			for (i=0; i< (strlen(buf)-strlen(string)); i++) {

				for (i2=0; i2<strlen(string); i2++) {
					buf2[i2]=buf[i+i2];
				}

				if (strcasecmp(buf2, string) == 0) {
					matched=TRUE;
					i=(strlen(buf)-strlen(string));
				}

			}
		}

		else {
			output = strstr(buf, string);

			if (output != NULL)
				matched = TRUE;
		}

		if (matched==TRUE) {

			if (v_option==FALSE && c_option==FALSE)
				if (n_option == FALSE)
					printf("%s", buf);
				else
					printf("[%d]%s", row_counter, buf);

			if (c_option==TRUE && v_option==FALSE)
				nr_matches++;

			matched = FALSE;
		}

		else { //NOT MATCHED

			if (v_option == TRUE && c_option==FALSE)
				if (n_option == FALSE)
					printf("%s", buf);
				else
					printf("[%d]%s", row_counter, buf);

			if (c_option==TRUE && v_option==TRUE)
				nr_matches++;
		}

	}
	if (c_option==TRUE)
		printf("\n----------%s:  %d", src, nr_matches);

	printf("\n");
	free(buf);
	if (i_options==TRUE)
		free(buf2);

}

void find(param **parameters){

	
	param *p = (*parameters);
	param *prm = (*parameters);
	char *src;
	short int p1= FALSE, p2=FALSE;
	

	//initialization of params to default values
	v_option=FALSE; //show not matched
	c_option=FALSE; //show only the number of matches
	n_option=FALSE; //show also line number: it has effciency only if c=false
	i_options=FALSE;
	
	while (p!= NULL) {

		if (p->type==0) {
			src = (char *)malloc(strlen(p->name));
			strcpy(src, p->name);
			p2 = TRUE;
			break;
		}
		else{
			/*short v_option=FALSE; //show not matched
	short c_option=FALSE; //show only the number of matches
	short n_option=FALSE; //show also line number: it has effciency only if c=false
	short i_options=TRUE; //ignore uppercase
	*/
			if (strcasecmp(p->name, "\\V") == 0)
								v_option=TRUE;
			else if(strcasecmp(p->name, "\\C") == 0)
						c_option=TRUE;
			else if(strcasecmp(p->name, "\\N") == 0)
					n_option=TRUE;
			else if(strcasecmp(p->name, "\\I") == 0)
					i_options = TRUE;
			else {
							printf("FIND : not valid argument\n");
							exit(1);
			}
		}
		
		p=p->next;
	}
	
	//TODO correct parse .c : jollychar "
	if (p2==TRUE) {
		
		while (prm != NULL) {
			
			if(prm->type == 1){
				
				if (strcasecmp(p->name, "\\V") == 0){
					prm=prm->next;
					continue;
				}
				else if(strcasecmp(p->name, "\\C") == 0){
					prm=prm->next;
					continue;
				}
				else if(strcasecmp(p->name, "\\N") == 0){
					prm=prm->next;
					continue;
				}
				else if(strcasecmp(p->name, "\\I") == 0){
					prm=prm->next;
					continue;
				}
				else{
						p1=TRUE; 
						my_find(prm->name,src);
				}
			}
			
			
		}
	}
	else{
		printf("1FIND: not correct parameter format\n");
		exit(1);
	}
	
	if(p1 == FALSE){
		printf("2FIND: not correct parameter format\n");
		free(src);
		exit(1);
	}
	
	
}
