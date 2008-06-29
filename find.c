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
#include <fcntl.h>
#include <ctype.h>
#include "parse.h"
#include "resource.h"

/*look at the content of the file, line by line, and print the different ones */
// *** receive string to be found, path name ***
void my_find(char *, char *);

/*look for paramters and launch my_find functions*/
// *** receive list with path and options ***
void find(param *parameters);

extern char *extract_double_quotes(char *);

short v_option; //show not matched
short c_option; //show only the number of matches
short n_option; //show also line number: it has effciency only if c=false
short i_options; //ignore uppercase


/*look at the content of the file, line by line, and print the different ones */
void my_find(char *string_to_search, char *src) {

	int row_counter=0, nr_matches=0, i=0, i2=0;
	FILE *fp;
	char *buf, *output, *buf2;
	char *string = (char *)malloc(strlen(string_to_search));

	strcpy(string,string_to_search);

	short matched=FALSE;

	if ( (fp = fopen(src, "r")) == NULL) {
		fprintf(stderr,"FIND: cannot access : %s: No such file or directory\n",
				src);
		exit(1);
		return;
	}

	for (i=0; i<strlen(src); i++) {
		src[i] = toupper(src[i]);
	}
	i=0;
	//if argument 1 doesn't begin with "" quotes,error. it seems impossible to emulate

	//TODO src uppercase
	if (c_option == FALSE)
		fprintf(stdout,"\n----------%s\n", src);

	buf = (char *) malloc((unsigned int)STRING_LENGTH);
	if (i_options==TRUE)
		buf2=(char *) malloc((unsigned int)strlen(string));

	while (fgets(buf, (int)STRING_LENGTH, fp) != NULL) {
		matched = FALSE;
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

			if (v_option==FALSE && c_option==FALSE) {
				if (n_option == FALSE) {
					fprintf(stdout,"%s", buf);
				} else {
					fprintf(stdout,"[%d]%s", row_counter, buf);
				}
			}

			if (c_option==TRUE && v_option==FALSE)
				nr_matches++;

			matched = FALSE;
		}

		else { //NOT MATCHED

			if (v_option == TRUE && c_option==FALSE) {
				if (n_option == FALSE) {
					fprintf(stdout,"%s", buf);
				} else {
					fprintf(stdout,"[%d]%s", row_counter, buf);
				}
			}

			if (c_option==TRUE && v_option==TRUE)
				nr_matches++;
		}

	}
	if (c_option==TRUE)
		fprintf(stdout,"\n----------%s:  %d", src, nr_matches);

	fprintf(stdout,"\n");
	fclose(fp);
	//free(buf);
	//if (i_options==TRUE)
	//	free(buf2);

}
/*look for paramters and launch my_find functions*/
void find(param *parameters) {

	param *p = parameters;
	char *buffer=(char *)malloc(MAXPATH);
	char *to_search;
	char *src;
	short int p1= FALSE, p2=FALSE; //p2 is the file, p1 is the string to search
	fd_set rds;
	struct timeval tv;
	int retval;

	FD_ZERO (&rds);

	if (p==NULL) {

		FD_SET(0,&rds);	// add read to select control
		tv.tv_sec=1;	// set timeout for select
		tv.tv_usec=0;

		retval = select(1,&rds,NULL,NULL, &tv); // select try to read input from stdin 
		// (or pipe) but only until timeout expires

		if (retval) // select read from stdin/pipe
		{
			if (FD_ISSET(0,&rds))
			{
				read(0,buffer,BUF_MAX/2);
				*(buffer+(strlen(buffer)-1))='\0';
				parameters = parse_options(buffer,0,0);
			}
		}
		else // timeout expires so nothing from input
		{
			fprintf(stderr, "find: missing file operand\n");
			fprintf(stderr, "Try \'help find\' for more information\n");
			exit(1);
		}
	}
	p = parameters;

	//initialization of params to default values
	v_option=FALSE; //show not matched
	c_option=FALSE; //show only the number of matches
	n_option=FALSE; //show also line number: it has effciency only if c=false
	i_options=FALSE;

	while (p!= NULL) {


		if (p->type==1) {

			/*short v_option=FALSE; //show not matched
			 short c_option=FALSE; //show only the number of matches
			 short n_option=FALSE; //show also line number: it has effciency only if c=false
			 short i_options=TRUE; //ignore uppercase
			 */
			if (strcasecmp(p->name, "\\V") == 0)
				v_option=TRUE;
			else if (strcasecmp(p->name, "\\C") == 0)
				c_option=TRUE;
			else if (strcasecmp(p->name, "\\N") == 0)
				n_option=TRUE;
			else if (strcasecmp(p->name, "\\I") == 0)
				i_options = TRUE;
			else if (p->name[0]=='"' && p->name[strlen(p->name)-1]=='"') {
				p=p->next;
				continue;
			} else {
				fprintf(stdout,"FIND : --%s-- not valid argument\n", p->name);
				exit(1);
			}
		}

		p=p->next;
	}

	//here looking for the string to search


	p=parameters;

	while (p != NULL) {

		if (p->type == 1) {

			if (strcasecmp(p->name, "\\V") == 0) {
				p=p->next;
				continue;
			} else if (strcasecmp(p->name, "\\C") == 0) {
				p=p->next;
				continue;
			} else if (strcasecmp(p->name, "\\N") == 0) {
				p=p->next;
				continue;
			} else if (strcasecmp(p->name, "\\I") == 0) {
				p=p->next;
				continue;
			} else {
				p1=TRUE;
				to_search
				= (char *)malloc(strlen(extract_double_quotes(p->name)));
				strcpy(to_search,extract_double_quotes(p->name));
				break;

			}
		}

		p=p->next;
	}

	p=parameters;
	if (p1==TRUE) {

		while (p!= NULL) {

			if (p->type==0) {

				src = (char *)malloc(strlen(p->name));
				strcpy(src, p->name);
				p2 = TRUE;

				my_find(to_search, src);
			}
			p=p->next;
		}

	}

	else {
		fprintf(stdout,"FIND: need string parameter\n");
		exit(1);
	}

	if (p2 == FALSE) {
		fprintf(stdout,"FIND: need file name\n");
		free(src);
		exit(1);
	}

}

