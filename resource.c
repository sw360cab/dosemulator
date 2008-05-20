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
#define MAXPATH 4096
#define TIMLEN 60

Resource *free_resources= NULL; /* Pointer to my free_list of events */

/* 
 **  Function:    void delete_event(Event **last,Event *event)
 **  Parameters:  Event **last  - Reference pointer for the list (the last
 **                               element)
 **               Event  *elem  - Record of type Event to be removed
 **  Return:      none
 **  Description: Remove the event 'elem' from the FES, leaving it in a
 **               correct state. The event must already be in the FES and it is
 **               not looked up. 
 */

void delete_resource(Resource **last, Resource *elem) {

	if (elem==NULL)
		return;
	if ((*last)==NULL)
		return;

	if ((*last)->next==(*last)) {
		/* There is a single event in the FES */
		if ( (*last)==elem) {
			(*last)=NULL;
		}
	} else {
		(elem->next)->prev = elem->prev;
		(elem->prev)->next = elem->next;
		if ((*last)==elem) {
			/* elem was the last event in the FES: update the FES reference */
			(*last) = elem->prev;
		}
	}

	/* Detach the record from the FES */
	elem->next = NULL;
	elem->prev = NULL;

	return;
}

Resource* delete_resource_POP(Resource **last, Resource *elem) {

	if (elem==NULL)
		return NULL;
	if ((*last)==NULL)
		return NULL;

	if ((*last)->next==(*last)) {
		/* There is a single event in the FES */
		if ((*last)==elem) {
			(*last)=NULL;
		}
	} else {
		(elem->next)->prev = elem->prev;
		(elem->prev)->next = elem->next;
		if ((*last)==elem) {
			/* elem was the last event in the FES: update the FES reference */
			(*last) = elem->prev;
		}
	}

	/* Detach the record from the FES */
	elem->next = NULL;
	elem->prev = NULL;

	return elem;
}

/* 
 **  Function:    void insert_event(Event **last, Event *elem)
 **  Parameters:  Event **last  - Reference pointer for the list (the last 
 **                               element)
 **               Event  *elem  - Record of type Event to be inserted
 **  Return:      none
 **  Description: Inserts 'elem' in the Event List referenced by **last
 */
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
	 ** elem is scheduled later than the last event in the list:
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
			/* elem must be the first event in the list:
			 insert elem after p (that is *last) */
			elem->prev = p;
			elem->next = p->next;
			(p->next)->prev = elem;
			p->next = elem;
		}
	}
	return;
}

/* 
 **  Function:    Event *get_event(Event **last)
 **  Parameters:  Event **last  - Reference pointer for the list (the last 
 **                               element)
 **  Return:      Pointer to the extracted event 
 **  Description: Returns the next scheduled event, removing it from the top of
 **               the FES. Returns NULL if the FES was empty
 */
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

/* 
 **  Function:	 Event *new_event(void)
 **  Parameters:  none
 **  Return:      Pointer to a new event
 **  Description: Returns a new event either allocating it or extracting it 
 **               from the Free List
 **  Side Effect: Extracts elements from the global Free List
 */
Resource *new_resource(void) {
	extern Resource *free_resources;
	Resource *p;

	if (free_resources==NULL) {
		if ((p=(Resource *)malloc(sizeof(Resource)))==NULL) {
			printf("Error  : Memory allocation error\n");
			exit(1);
		}
		//p->time = (Time)0;
		p->name = "";
		insert_resource(&free_resources, p);
	}
	return (get_resource(&free_resources)); /* Returns the first element of the
	 Free List */
}

/* 
 **  Function:	 void release_event(Event *elem)
 **  Parameters:  Event *elem - Event to be released
 **  Return:      None
 **  Description: Releases the record of the event, inserting it in the
 **               Free List
 **  Side Effect: Inserts elements in the global Free List
 */
void release_resource(Resource *elem) {
	extern Resource *free_resources;
	//elem->time = (Time)MAX_TIME;   
	/* In this way the record is at the bottom of the Free List */
	strcpy(elem->name, "zzzzz");
	insert_resource(&free_resources, elem);
}

void timespec2string(struct timespec *ts, char buffer[], int len)
/* It reads the time from ts and puts it in buffer (of size len) */
/* as a string */
{
	//ctime_r(&(ts->tv_sec), buffer, len);
	ctime_r(&(ts->tv_sec), buffer);
	/* ctime_r terminates the time with a carriage return. We eliminate it.*/
	/* We report time in microseconds. That is the precision on our system.*/
	//sprintf(&buffer[24], " and %6d microseconds", (ts->tv_nsec)/1000);
	sprintf(&buffer[24], " %d ", (int)ts->tv_sec);
}
  

void stdprint(Resource *res_list, int *dir, int *files, int *file_size) {

	struct timespec t;
	struct tm * timeinfo;
	time_t rawtime;
	unsigned int l = 25;
	int i=0;
	char *dirindication = (char *)malloc(l);
	char buffer[TIMLEN];
	char *sizeandname;

	t = res_list->status.st_mtim;
	rawtime = t.tv_sec;
	timeinfo = localtime((long *)&rawtime);
	strftime(buffer, 80, "%d/%m/%Y  %H.%M    ", timeinfo);
	if (res_list->type == 4) {
		dirindication="<DIR>               ";
		sizeandname = malloc(1);
		sizeandname = " ";
		(*dir)++;
	} else {

		sprintf(dirindication, "%d", (int)res_list->status.st_size);
		sizeandname = malloc(21-strlen(dirindication)+1);
		for (i=0; i<( 21-strlen(dirindication)); i++) {
			sizeandname[i]=' ';
		}
		
		(*files)++;
		*file_size+= (int)res_list->status.st_size;

	}

	printf("%s %s %s %s\n", buffer, sizeandname , dirindication, res_list->name);
	
}

void stdprint_parents(char *path) {

	struct stat status;
	struct timespec t;
	struct tm * timeinfo;
	time_t rawtime;
	char buffer[TIMLEN];
	char *sizeandname;
	int *p;
	unsigned int l = 25;
	char *dirindication = (char *)malloc(l);
	

	if ( (p=(int *)open(path, O_EXCL)) != NULL) {

		if (fstat((int)p, &status) == 0) {
			//TODO build a function that returns the date and substitute it in all prints 
			t = status.st_mtim;
			rawtime = t.tv_sec;
			timeinfo = localtime( &rawtime);
			strftime(buffer, 80, "%d/%m/%Y  %H.%M    ", timeinfo);
			dirindication="<DIR>               ";
			sizeandname = malloc(1);
			sizeandname = " ";

			printf("%s %s %s .\n", buffer, sizeandname, dirindication);
		}
	} else
		printf("dir: cannot access : %s: No such file or directory\n", path);

	strcat(path, "/..");
	if ( (p=open(path, O_EXCL)) != NULL) {

		if (fstat((int)p, &status) == 0) {
			//TODO build a function that returns the date and substitute it in all prints 
			t = status.st_mtim;
			rawtime = t.tv_sec;
			timeinfo = localtime( &rawtime);
			strftime(buffer, 80, "%d/%m/%Y  %H.%M    ", timeinfo);
			dirindication="<DIR>               ";
			sizeandname = malloc(1);
			sizeandname = " ";

			printf("%s %s %s ..\n", buffer, sizeandname, dirindication);
		}
	} else
		printf("dir: cannot access : %s: No such file or directory\n", path);

}

Resource *print_list(Resource *res_list, char *path, short recursive) {
	Resource *last, *first, *res;
	Resource *dirs_list=NULL;

	int files = (int) malloc(sizeof(int));
	int totfilesize =(int) malloc(sizeof(int));
	int dirs =(int) malloc(sizeof(int));
	char *temp_path =(char *) malloc(MAXPATH);
	struct statvfs status_space;
	

	files = 0;
	totfilesize=0;
	dirs = 2;
	
	printf("\n Directory di %s\n\n", path);
 
	last= res_list;
	temp_path =(char *) malloc((unsigned int)MAXPATH);
	strcpy(temp_path, path);
	stdprint_parents(temp_path);

	if (res_list != NULL) {
		if (res_list->next != NULL) {
			first=res_list->next;
			res_list=res_list->next;

			while (res_list->next!=first) {

				

				stdprint(res_list, &dirs, &files, &totfilesize);

				if (recursive == 1 && res_list->type==4) {

					if (!strcmp(res_list->name, "")==0) {
 
						res =(Resource *) create_res(res_list->status, res_list->name,
								res_list->type, path);
						insert_resource(&dirs_list, res);
					}
				}

				res_list=res_list->next;
			}

			stdprint(res_list, &dirs, &files, &totfilesize);
/*
			printf("               %d File(s)    %d bytes\n", files,
					totfilesize);
			printf("               %d Folder(s)\n", dirs);
*/
			if (recursive == 1 && res_list->type==4) {

				if (!strcmp(res_list->name, "")==0) {
					res = create_res(res_list->status, res_list->name,
							res_list->type, path);
					insert_resource(&dirs_list, res_list);
				}
			}
			res_list=last;
		}
	} else
		dirs_list = NULL;
	
	statvfs(path,&status_space);
	printf("               %d File(s)    %d bytes\n", files,
			totfilesize);
	printf("               %d Folder(s)  %d available blocks\n\n", dirs, (int) status_space.f_bfree);
	//TODO block size ? 512 ? 
	

	return dirs_list;
}
