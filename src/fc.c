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

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "parse.h"
#include "resource.h"

/*print differences between list of strings*/
// *** receive source resources , dest path resources ***
void print_differences(Resource *, Resource *);

/*reads content of 2 files and build 2 lists with the strings contained in the file  */
// *** receive source path name, dest path name ***
void my_fc(char *, char *);

/* check for parameters and launch my_fc function*/
// *** receive list with path and options ***
void fc(param *);

short show_line_option= FALSE;
short case_sensitive= TRUE;

int smaller_lines=0;
int bigger_lines=0;

//paramters are already the firts in the list
/*print differences between list of strings*/
void print_differences(Resource *smaller, Resource *bigger) {

	Resource  *copy_smaller, *copy_bigger;
	Resource *last_smaller, *last_bigger;

	int i=0;

	//fprintf(stdout,"%s smaller than %s \n",smaller->path,bigger->path);
	copy_smaller = smaller->next;//it was smaller
	copy_bigger = bigger->next; //it was bigger

	last_smaller = smaller->prev;
	last_bigger = bigger->prev;

	//fprintf(stdout,"%s %d lines, %s %d lines\n",smaller->path, smaller_lines,bigger->path,bigger_lines);

	//first= mark all equals lines 

	if ( (int)copy_smaller->flag != TRUE && (int)copy_smaller != TRUE) {
		
		for (i=0; i<smaller_lines; i++) {
			
			if (case_sensitive==TRUE) {

				//fprintf(stdout,"comparing \n %s \nand \n %s\n",copy_smaller->name, copy_bigger->name);
				if (strcmp(copy_smaller->name, copy_bigger->name) == 0) {

					copy_smaller->flag=TRUE;
					copy_bigger->flag= TRUE;

				}

			} else {

				if (strcasecmp(copy_smaller->name, copy_bigger->name) == 0) {

					copy_smaller->flag=TRUE;
					copy_bigger->flag=TRUE;

				}

			}

			copy_smaller=copy_smaller->next;
			copy_bigger=copy_bigger->next;
		}
	}

	//second = print all not marked remaining lines (the ones who differ)

	copy_smaller=smaller; //it was smaller 
	copy_bigger=bigger->next;   //it was bigger

	fprintf(stdout,"\n****** %s *******\n\n", smaller->path);
	i=0;

	for (i=0; i<smaller_lines; i++) {
		
		if (copy_smaller->flag!=TRUE) {

			if (show_line_option==FALSE)
				fprintf(stdout,"%s", copy_smaller->name);
			else
				fprintf(stdout,"%d   %s", copy_smaller->type, copy_smaller->name);
		}
		copy_smaller = copy_smaller->next;
	}



	fprintf(stdout,"\n****** %s *******\n\n", bigger->path);

	i=0;


	for (i=0; i<bigger_lines; i++) {
		
		if (copy_bigger->flag!=TRUE) {

			if (show_line_option==FALSE)
				fprintf(stdout,"%s", copy_bigger->name);
			else
				fprintf(stdout,"%d   %s", copy_bigger->type, copy_bigger->name);
		}

		copy_bigger = copy_bigger->next;
	}



	fprintf(stdout,"\n");
}

/*reads content of 2 files and build 2 lists with the strings contained in the file  */
void my_fc(char *path1, char *path2) {

	//TODO use lioy function to read and not fgets
	FILE *fp1, *fp2, *fp_bigger, *fp_smaller;
	//int fp1,fp2,fp_bigger,fp_smaller;
	int row = 1;
	int count_lines1=0, count_lines2=0;

	//here i use reource as line container: name is the string, and type is number line
	//	Resource *list_bigger=(Resource *) malloc(sizeof( Resource));
	//	Resource *list_smaller=(Resource *) malloc(sizeof(Resource));

	Resource *list_bigger=NULL;
	Resource *list_smaller=NULL;

	Resource *res_temp;

	char *buf_bigger, *buf_smaller;
	char *path_bigger, *path_smaller;

	buf_bigger = (char *)malloc( (unsigned short) STRING_LENGTH + 1);
	buf_smaller = (char *)malloc( (unsigned short) STRING_LENGTH + 1);

	struct stat status_smaller;
	struct stat status_bigger;



	if ( (fp1=fopen(path1, "r")) == NULL) {

		fprintf(stderr,"FC: %s no such file\n",path1);
		exit(1);
	}
	if ( (fp2=fopen(path2, "r")) == NULL) {

		fprintf(stderr,"FC: %s no such file\n",path1);
		exit(1);
	}



	//count number of lines to determines which one is the bigger
	while (fgets(buf_smaller, (int)STRING_LENGTH, fp1) != NULL) {

		count_lines1++;

	}


	while (fgets(buf_smaller, (int)STRING_LENGTH, fp2) != NULL) {

		count_lines2++;

	}



	//old compare:
	// if (status1.st_size >= status2.st_size)
	if (count_lines1 >= count_lines2) {

		//		fp_bigger = fp1;
		//		fp_smaller = fp2;

		path_smaller = (char *)malloc(strlen(path2));
		strcpy(path_smaller, path2);

		path_bigger = (char *)malloc(strlen(path1));
		strcpy(path_bigger, path1);

		smaller_lines = count_lines2;
		bigger_lines = count_lines1;

		stat(path1, &status_bigger);
		stat(path2, &status_smaller);

	} else {

		//		fp_bigger = fp2;
		//		fp_smaller = fp1;

		path_smaller = (char *)malloc(strlen(path1));
		strcpy(path_smaller, path1);

		path_bigger = (char *)malloc(strlen(path2));
		strcpy(path_bigger, path2);

		smaller_lines = count_lines1;
		bigger_lines = count_lines2;

		stat(path1, &status_smaller);
		stat(path2, &status_bigger);

	}

	fclose(fp1);
	fclose(fp2);


	fp_smaller=fopen(path_smaller, "r");
	fp_bigger=fopen(path_bigger, "r");



	while (fgets(buf_smaller, (int)STRING_LENGTH/2, fp_smaller) != NULL) {

		res_temp = new_resource();
		res_temp =(Resource *) create_res(status_smaller, buf_smaller, row, path_smaller);
		insert_resource_order_by_type(&list_smaller, res_temp);//it was insert_resource
		//fprintf(stdout,"inserted line %s of %s\n",res_temp->name,res_temp->path);
		row++;

	}

	if (row==1) {

		res_temp = new_resource();
		res_temp = (Resource *)create_res(status_smaller, "", row, path_smaller);
		res_temp->flag = TRUE;
		insert_resource(&list_smaller, res_temp);

	}

	//fprintf(stdout,"fgets on %s \n",list_smaller->path);


	row=1;

	while (fgets(buf_bigger, (int)STRING_LENGTH/2, fp_bigger) != NULL) {

		res_temp = new_resource() ;
		res_temp = (Resource *)create_res(status_bigger, buf_bigger, row, path_bigger);
		insert_resource_order_by_type(&list_bigger, res_temp);//it was insert_resource
		row++;

	}

	if (row==1) {

		res_temp = new_resource();
		res_temp = (Resource *)create_res(status_bigger, "", row, path_bigger);
		res_temp->flag=TRUE;
		insert_resource(&list_bigger, res_temp);

	}



	fclose(fp_smaller);
	fclose(fp_bigger);

	//fprintf(stdout,"launching %s and %s\n",list_smaller->path,list_bigger->path);
	print_differences(list_smaller, list_bigger);

}

/* check for parameters and launch my_fc function*/
void fc(param *parameters) {

	param *p = parameters;
	char *file1, *file2;
	short int count_files = 0;
	char *buffer=(char *)malloc(MAXPATH);
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
			fprintf(stderr, "fc: missing file operand\n");
			fprintf(stderr, "Try \'help fc\' for more information\n");
			exit(1);
		}
	}
	p = parameters;

	while (p!= NULL) {

		if (p->type == 0) {
			count_files++;

			if (count_files == 1) {

				file1 = (char *) malloc((unsigned int)strlen(p->name));
				strcpy(file1, p->name);

			} else if (count_files == 2) {

				file2 = (char *) malloc((unsigned int)strlen(p->name));
				strcpy(file2, p->name);

			}

		} else if (p->type == 1) {

			if (strcasecmp(p->name, "\\N") == 0)
				show_line_option = TRUE;
			else if (strcasecmp(p->name, "\\C") == 0)
				case_sensitive= FALSE;
			else {

				fprintf(stderr,"FC: not valid argument\n");
				exit(1);
			}

		}

		p=p->next;

	}

	if (count_files < 2) {

		fprintf(stderr, "FC: bad number of arguments\n");
		exit(1);

	} else

		my_fc(file1, file2);

}

