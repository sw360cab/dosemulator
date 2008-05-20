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

//TODO how to define a good string length ?
#define STRING_LENGTH 1024
#define TRUE 1
#define FALSE 0

void find(char *string, char *src) {

	int row_counter=0, nr_matches=0, i=0, i2=0;
	FILE *fp;
	char *buf, *output, *buf2;
	short v_option=FALSE; //show not matched
	short c_option=FALSE; //show only the number of matches
	short n_option=FALSE; //show also line number: it has effciency only if c=false
	short i_options=TRUE; //ignore uppercase
	short matched=FALSE;

	if ( (fp = fopen(src, "r")) == NULL) {
		fprintf(stderr,"find: cannot access : %s: No such file or directory\n",
		src);
		exit(1);
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

		//if (output != NULL) { //MATCHED
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

int main(int argc, char **argv) {

	find(argv[1], argv[2]);

}
