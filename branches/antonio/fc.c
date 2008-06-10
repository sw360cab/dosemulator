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
//TODO Per i comandi che non accettano di essere senza parametri metti subito all'inizio della funzione
/*
 if(list==NULL)
 {
 fprintf(stderr, "md: missing file operand\n");
 fprintf(stderr, "Try \'help md\' for more information\n");
 exit(1);
 }
 */
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include "parse.h"
#include "resource.h"
#define STRING_LENGTH 1024
short show_line_option= FALSE;

void print_differences(Resource *smaller, Resource *bigger) {

	Resource *first_smaller, *first_bigger;

	if (smaller==NULL || bigger==NULL) {
		printf("FC: No differerences found\n");
		return;
	}

	first_smaller = smaller->next;
	first_bigger = bigger->next;

	smaller = smaller->next;
	printf("\n***** %s ******\n\n", smaller->path);

	while (smaller->next != first_smaller) {

		if (show_line_option==FALSE)
			printf("%s\n", smaller->name);
		else
			printf("%d   %s\n", smaller->type, smaller->name);

		smaller = smaller->next;
	}
	if (show_line_option==FALSE)
		printf("%s\n", smaller->name);
	else
		printf("%d   %s\n", smaller->type, smaller->name);

	bigger = bigger->next;
	printf("\n***** %s ******\n\n", bigger->path);

	while (bigger->next != first_bigger) {

		if (show_line_option==FALSE)
			printf("%s\n", bigger->name);
		else
			printf("%d   %s\n", bigger->type, bigger->name);

		bigger = bigger->next;
	}
	if (show_line_option==FALSE)
		printf("%s\n", bigger->name);
	else
		printf("%d   %s\n", bigger->type, bigger->name);

}

void my_fc(char *path1, char *path2) {

	FILE *fp1, *fp2, *fp_bigger, *fp_smaller;
	int row = 1;

	//here i use reource as line container: name is the string, and type is number line
	Resource *list_bigger=NULL;
	Resource *list_smaller=NULL;

	Resource *res_temp;

	char *buf_bigger, *buf_smaller, *line;
	char *path_bigger, *path_smaller;
	struct stat status1;
	struct stat status2;

	if ( (fp1=fopen(path1, "r")) == NULL) {

		fprintf(stderr,"FC: %s no such file",path1);
		exit(1);
	}
	if ( (fp2=fopen(path2, "r")) == NULL) {

		fprintf(stderr,"FC: %s no such file",path1);
		exit(1);
	}

	if (stat(path1, &status1) != 0) {
		fprintf(stderr,"FC: Cannot open file %s: Permission denied\n", path1);
        exit(1);
	}
	if (stat(path2, &status2)==0) {
		fprintf(stderr,"FC: Cannot open file %s: Permission denied\n", path2);
		exit(1);
	}

	if (status1.st_size >= status2.st_size) {

		fp_bigger = fp1;
		fp_smaller = fp2;

		path_smaller = (char *)malloc(strlen(path2));
		strcpy(path_smaller, path2);

		path_bigger = (char *)malloc(strlen(path1));
		strcpy(path_bigger, path1);

	} else {

		fp_bigger = fp2;
		fp_smaller = fp1;

		path_smaller = (char *)malloc(strlen(path1));
		strcpy(path_smaller, path1);

		path_bigger = (char *)malloc(strlen(path2));
		strcpy(path_bigger, path2);

	}

	while (fgets(buf_smaller, (int)STRING_LENGTH, fp_smaller) != NULL) {

		fgets(buf_bigger, (int)STRING_LENGTH, fp_bigger);

		if (buf_bigger == NULL) {
			fprintf(stderr,"FC: error reading file\n");
			exit(1);
		}

		if (strcmp(buf_bigger, buf_smaller) != 0) {

			res_temp = new_resource() ;
			strcpy(res_temp->name, buf_bigger);
			res_temp->type=row;
			strcpy(res_temp->path, path_bigger);
			insert_resource_order_by_type(&list_bigger, res_temp);

			res_temp = new_resource();
			strcpy(res_temp->name, buf_smaller);
			strcpy(res_temp->path, path_smaller);
			res_temp->type=row;

			insert_resource(&list_smaller, res_temp);

		}

		row++;
	}

	while (fgets(buf_bigger, (int)STRING_LENGTH, fp_bigger) != NULL) {

		res_temp = new_resource() ;
		strcpy(res_temp->name, buf_bigger);
		res_temp->type=row;
		strcpy(res_temp->path, path_bigger);
		insert_resource(&list_bigger, res_temp);

		row++;

	}

	close(fp1);
	close(fp2);

	print_differences(list_smaller, list_bigger);

}

void fc(param **parameters) {

	param *p = (*parameters);

	if (p==NULL) {
		fprintf(stderr, "md: missing file operand\n");
		fprintf(stderr, "Try \'help md\' for more information\n");
		exit(1);
	}

	char *file1, *file2;
	short int count_files = 0;

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

