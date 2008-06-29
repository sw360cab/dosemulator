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
 
#include  <sys/types.h>
#include <sys/statvfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "resource.h"
#include "parse.h"
#define MAXPATH 4096
#define TIMLEN 60

Resource *free_resources= NULL; /* Pointer to my free_list of events */

/*converts a decimal number into a binary one as array of chars */
void dec2bin(long decimal, char *binary) {

	int k = 0, n = 0;

	int neg_flag = 0;

	int remain;

	int old_decimal; // for test

	char temp[80];
	// take care of negative input

	if (decimal < 0)

	{

		decimal = -decimal;

		neg_flag = 1;

	}

	do

	{

		old_decimal = decimal; // for test

		remain = decimal % 2;

		// whittle down the decimal number

		decimal = decimal / 2;

		// this is a test to show the action

		//fprintf(stdout,"%d/2 = %d remainder = %d\n", old_decimal, decimal, remain);

		// converts digit 0 or 1 to character '0' or '1'

		temp[k++] = remain + '0';

	} while (decimal > 0);
	if (neg_flag)

		temp[k++] = '-'; // add - sign

	else

		temp[k++] = ' '; // space


	// reverse the spelling

	while (k >= 0)

		binary[n++] = temp[--k];

	binary[n-1] = 0; // end with NULL

}

/*given a string , it extracts double quotes from beginning and end*/
char *extract_double_quotes(char *src){

	char *doublequotes, *temp;
	int i;

	if(src[0]=='"' && src[strlen(src)-1]=='"'){

		doublequotes = (char *)malloc(strlen(src)-2);
		temp = (char *)malloc(1);

		for (i=1; i<strlen(src)-1; i++) {

			sprintf(temp, "%c", src[i]);
			if (i==1)
				strcpy(doublequotes, temp);
			else
				strcat(doublequotes, temp);
		}

		return doublequotes;
	}
	else
		return src;

}

/*tells wheter resource is read only*/
int is_read_only(long st_mode) {

	char binary[80];
	int result=0;

	dec2bin((long)st_mode, (char *) &binary);

	//8 9 10 - 11 12 13 - 14 15 16

	//char is 48, char 1 is 49
	if (binary[9]==48 &&binary[10]==48 && binary[12]==48 && binary[13]==48
			&& binary[15]==48 && binary[16]==48) {

		if (binary[8]==49 || binary[11]==49 || binary[14]==49) {

			result=1;

		}
	}

	else
		(result= 0);
	/*fprintf(stdout,"%c%c%c %c%c%c %c%c%c = %d\n ", binary[8], binary[9], binary[10],
			binary[11], binary[12], binary[13], binary[14], binary[15],
			binary[16], result);
	 */
	return result;

}

/* build the full path of a resource, given its name and folder parent containing path */
char *build_path(char *parent_path, char* resource_name) {

	char *new_path = malloc((unsigned int) MAXPATH);
	strcpy(new_path, parent_path);
	strcat(new_path, "/");
	strcat(new_path, resource_name);

	return new_path;

}
/*create a resource with parameters*/
Resource *create_res(struct stat status, char res_name[], unsigned char type,
		char *path) {

	Resource *res;
	char *name, *new_path;

	name =(char *) malloc((unsigned int)strlen(res_name));
	new_path =(char *)malloc((unsigned int)MAXPATH);

	strcpy(name, res_name);
	strcpy(new_path, path);

	res = new_resource();
	res->status = status;
	res->name = name;
	res->type = type;
	res->path = new_path;
	res->flag = FALSE;

	//can't free none
	//free(name);
	//free(new_path);
	return res;

}

/* Delete a resource */
void delete_resource(Resource **last, Resource *elem) {

	if (elem==NULL)
		return;
	if ((*last)==NULL)
		return;

	if ((*last)->next==(*last)) {
		/* There is a single resource */
		if ( (*last)==elem) {
			(*last)=NULL;
		}
	} else {
		(elem->next)->prev = elem->prev;
		(elem->prev)->next = elem->next;
		if ((*last)==elem) {
			/* elem was the last resource: update reference */
			(*last) = elem->prev;
		}
	}

	/* Detach the record from */
	elem->next = NULL;
	elem->prev = NULL;

	return;
}
/* pop an element from list */
Resource* delete_resource_POP(Resource **last, Resource *elem) {

	if (elem==NULL)
		return NULL;
	if ((*last)==NULL)
		return NULL;

	if ((*last)->next==(*last)) {
		/* There is a single resource */
		if ((*last)==elem) {
			(*last)=NULL;
		}
	} else {
		(elem->next)->prev = elem->prev;
		(elem->prev)->next = elem->next;
		if ((*last)==elem) {
			/* elem was the last resource: update reference */
			(*last) = elem->prev;
		}
	}

	/* Detach the record from */
	elem->next = NULL;
	elem->prev = NULL;

	return elem;
}

/* Inserts a resource ordered */
void insert_resource_order_by_type(Resource **last, Resource *elem) {
	Resource *p;
	if (elem==NULL)
		return; /* Nothing to do */

	/* The Event List is empty: elem becomes the Event List */
	if ((*last)==NULL) {
		(*last) = elem;
		(*last)->next = elem;
		(*last)->prev = elem;
		return;
	}

	/* 
	 ** elem is scheduled later than the last resource in the list:
	 ** it is inserted at the end and becomes the new last
	 */
	//if (elem->time >= (*last)->time)
	if (elem->type >= (*last)->type) {
		elem->prev = (*last);
		elem->next = (*last)->next;
		((*last)->next)->prev = elem;
		(*last)->next = elem;
		(*last) = elem;
	} else
		/* 
		 ** lookup the correct position for elem, starting from the end
		 ** of the list, i.e. following the prev pointer
		 */
	{
		p = (*last);
		//while (elem->time < p->time && p->prev!=(*last))
		while (elem->type < p->type && p->prev!=(*last))
			p = p->prev;
		//if (elem->time < p->time)
		if (elem->type < p->type) {
			/* p is the first element scheduled after elem: 
			 insert elem immediately before it*/
			elem->prev = p->prev;
			elem->next = p;
			(p->prev)->next = elem;
			p->prev = elem;
		} else {
			/* elem must be the first resource in the list:
			 insert elem after p (that is *last) */
			elem->prev = p;
			elem->next = p->next;
			(p->next)->prev = elem;
			p->next = elem;
		}
	}
	return;
}

/* Inserts a resource */
void insert_resource(Resource **last, Resource *elem) {
	Resource *p;
	if (elem==NULL)
		return; /* Nothing to do */

	/* The Event List is empty: elem becomes the Event List */
	if ((*last)==NULL) {
		(*last) = elem;
		(*last)->next = elem;
		(*last)->prev = elem;
		return;
	}

	/* 
	 ** elem is scheduled later than the last resource in the list:
	 ** it is inserted at the end and becomes the new last
	 */
	//if (elem->time >= (*last)->time)
	if (strcmp(elem->name, (*last)->name)>=0) {
		elem->prev = (*last);
		elem->next = (*last)->next;
		((*last)->next)->prev = elem;
		(*last)->next = elem;
		(*last) = elem;
	} else
		/* 
		 ** lookup the correct position for elem, starting from the end
		 ** of the list, i.e. following the prev pointer
		 */
	{
		p = (*last);
		//while (elem->time < p->time && p->prev!=(*last))
		while ( (strcmp(elem->name, p->name) < 0) && p->prev!=(*last))
			p = p->prev;
		//if (elem->time < p->time)
		if (strcmp(elem->name, p->name) < 0) {
			/* p is the first element scheduled after elem: 
			 insert elem immediately before it*/
			elem->prev = p->prev;
			elem->next = p;
			(p->prev)->next = elem;
			p->prev = elem;
		} else {
			/* elem must be the first resource in the list:
			 insert elem after p (that is *last) */
			elem->prev = p;
			elem->next = p->next;
			(p->next)->prev = elem;
			p->next = elem;
		}
	}
	return;
}

/* Returns the first resource */
Resource *get_resource(Resource **last) {
	Resource *p;
	if ((*last)==NULL)
		return NULL; /* The Event List is empty */
	if ((*last)->next==(*last)) {
		/* I'm removing the only element from the Event List */
		p = (*last);
		(*last) = NULL;
	} else {
		/* Extract the first element (the successor of last) */
		p = (*last)->next;
		(p->next)->prev = (*last);
		(*last)->next = p->next;
	}
	return p;
}

/* Returns a new resource */
Resource *new_resource(void) {
	extern Resource *free_resources;
	Resource *p;

	if (free_resources==NULL) {
		if ((p=(Resource *)malloc(sizeof(Resource)))==NULL) {
			fprintf(stdout,"Error  : Memory allocation error\n");
			exit(1);
		}
		//p->time = (Time)0;
		p->name = "";
		insert_resource(&free_resources, p);
	}
	return (get_resource(&free_resources)); /* Returns the first element of the
	 Free List */
}

/* Releases a resource */
void release_resource(Resource *elem) {
	extern Resource *free_resources;
	//elem->time = (Time)MAX_TIME;   
	/* In this way the record is at the bottom of the Free List */
	strcpy(elem->name, "zzzzz");
	insert_resource(&free_resources, elem);
}

/* It reads the time from ts and puts it in buffer (of size len) */
/* as a string */
void timespec2string(struct timespec *ts, char buffer[], int len){
	//ctime_r(&(ts->tv_sec), buffer, len);
	ctime_r(&(ts->tv_sec), buffer);
	/* ctime_r terminates the time with a carriage return. We eliminate it.*/
	/* We report time in microseconds. That is the precision on our system.*/
	//sprintf(&buffer[24], " and %6d microseconds", (ts->tv_nsec)/1000);
	sprintf(&buffer[24], " %d ", (int)ts->tv_sec);
}

/* standard resource info printing */
void stdprint(Resource *res_list, int *dir, int *files, int *file_size) {

	struct timespec t;
	struct tm * timeinfo;
	time_t rawtime;
	unsigned int l = 23;//21
	int i=0;
	char *dirindication = (char *)malloc(l);
	char buffer[TIMLEN];
	char *sizeandname;
	int size=0;
	char space = ' ';
	char *temp = (char *)malloc(2);


	t = res_list->status.st_mtim;
	rawtime = t.tv_sec;
	timeinfo = localtime((long *)&rawtime);
	strftime(buffer, 80, "%d/%m/%Y  %H.%M    ", timeinfo);
	if (res_list->type == 4) {
		strcpy(dirindication,"<DIR>               ");
		//dirindication="<DIR>               ";
		sizeandname = malloc(2);
		strcpy(sizeandname," ");
		//sizeandname = " ";
		(*dir)++;

		fprintf(stdout,"%s %s %s %s\n", buffer, sizeandname, dirindication, res_list->name);

	} else {

		sprintf(dirindication, "%d", (int)res_list->status.st_size);
		size = strlen(dirindication);

		//if(size == 0)
		sizeandname = (char *) malloc(22);//22
		//else
		//	sizeandname =(char *) malloc(21-size+2);


		for (i=0; i<20-size+1; i++) {//<21-size+1

			sprintf(temp,"%c",space);
			if(i==0)
				strcpy(sizeandname,temp);
			else
				strcat(sizeandname,temp);

			//sizeandname[i]=' ';


		}

		//printf("dirindication: %s--,%d\nsizeandname: %s--,%d and theoric : %d\n",dirindication,size ,sizeandname,strlen(sizeandname),21-strlen(dirindication));
		(*files)++;
		*file_size+= (int)res_list->status.st_size;

		fprintf(stdout,"%s %s %s %s\n", buffer, sizeandname, dirindication, res_list->name);
	}


	//fprintf(stdout,"%s %s %s %s\n", buffer, sizeandname, dirindication, res_list->name);

}

/* time info print of folder . and .. */
void strdprint_time_parents(struct stat status, int father) {

	struct timespec t;
	struct tm * timeinfo;
	time_t rawtime;
	char buffer[TIMLEN];
	char *sizeandname;
	unsigned int l = 25;
	char *dirindication = (char *)malloc(l);

	t = status.st_mtim;
	rawtime = t.tv_sec;
	timeinfo = localtime( &rawtime);
	strftime(buffer, 80, "%d/%m/%Y  %H.%M    ", timeinfo);
	dirindication="<DIR>               ";
	sizeandname = malloc(1);
	sizeandname = " ";

	if (father==1)
		fprintf(stdout,"%s %s %s ..\n", buffer, sizeandname, dirindication);
	else
		fprintf(stdout,"%s %s %s .\n", buffer, sizeandname, dirindication);

	//can't free
}

/* folder . and .. info print */
void stdprint_parents(char *path) {

	struct stat status;
	int p;

	if ( (p=open(path, O_RDONLY)) != -1) {

		if (stat(path, &status) == 0) {

			strdprint_time_parents(status, 0);

		}
		close(p);
	} else
		fprintf(stdout,"DIR: problems opening : %s: \n", path);

	strcat(path, "/..");
	if ( (p=open(path, O_RDONLY)) != -1) {

		if (stat(path, &status) == 0) {

			strdprint_time_parents(status, 1);
		}
		close(p);
	} else
		fprintf(stdout,"DIR: problems opening : %s: \n", path);

}
/*print a list of resources - the third argument is a generic pointer parameter, to be used  through a cast to the specific type */
Resource *print_list(Resource *res_list, char *path, char *options) {
	Resource *last, *first, *res;
	Resource *dirs_list=NULL;
	int files = (int) malloc(sizeof(int));
	int totfilesize =(int) malloc(sizeof(int));
	int dirs =(int) malloc(sizeof(int));
	int recursive = 0;
	char *temp_path =(char *) malloc(MAXPATH);
	struct statvfs status_space;
	param *prms = (param *)options;
	dirs = 2;

	while (prms != NULL) {

		if (strcasecmp(prms->name, "\\S")== 0)
			recursive = 1;
		else if (strcasecmp(prms->name, "\\AH") == 0)
			dirs = 0;
		else if (strcasecmp(prms->name, "\\AR") == 0)
			dirs = 0;
		else if (strcasecmp(prms->name, "\\A-D") == 0)
			dirs = 0;

		prms = prms->next;
	}

	files = 0;
	totfilesize=0;

	fprintf(stdout,"\n Directory of %s\n\n", path);

	last= res_list;
	temp_path =(char *) malloc((unsigned int)MAXPATH);
	strcpy(temp_path, path);

	//if i don't have to show dirs, dirs is setted to zero and i don't print parent folders
	if (dirs == 2)
		stdprint_parents(temp_path);

	if (res_list != NULL) {
		if (res_list->next != NULL) {
			first=res_list->next;
			res_list=res_list->next;

			while (res_list->next!=first) {

				stdprint(res_list, &dirs, &files, &totfilesize);

				if (recursive == 1 && res_list->type==4) {

					if (!strcmp(res_list->name, "")==0) {

						res =(Resource *) create_res(res_list->status,
								res_list->name, res_list->type, path);
						insert_resource(&dirs_list, res);
					}
				}

				res_list=res_list->next;
			}

			stdprint(res_list, &dirs, &files, &totfilesize);
			/*
			 fprintf(stdout,"               %d File(s)    %d bytes\n", files,
			 totfilesize);
			 fprintf(stdout,"               %d Folder(s)\n", dirs);
			 */
			if (recursive == 1 && res_list->type==4) {

				if (!strcmp(res_list->name, "")==0) {
					res =(Resource *) create_res(res_list->status,
							res_list->name, res_list->type, path);
					insert_resource(&dirs_list, res_list);
				}
			}
			res_list=last;
		}
	} else
		dirs_list = NULL;

	statvfs(path, &status_space);
	fprintf(stdout,"               %d File(s)    %d bytes\n", files, totfilesize);
	fprintf(stdout,"               %d Folder(s)  %d available blocks\n\n", dirs,
			(int) status_space.f_bfree);

	return dirs_list;
}
