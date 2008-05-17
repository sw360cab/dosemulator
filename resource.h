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


#include <sys/stat.h>  

typedef struct resource_unit Resource;


struct resource_unit
      {
	Resource *next;	/* Next event in the list */
	Resource *prev;    /* Previous event in the list */
	char *name;     /* name of the resource */
	char *path;     /*path of the resource*/
	unsigned char type;     /* Type of resource, 4 =  directory , 8 = file */
	/* Optional fields, depending on the events to be simulated */
	//int   param1;   /* Generic integer parameter */
	//int   param2;   /* Generic integer parameter */
	//double param3;  /* Generic double  parameter */
	struct stat status; /* The result of the fstatus */
	
	//char  *record; /* Generic pointer parameter, to be used  through a cast to the specific type   */
	//Record *record;
      };

/* Prototypes */
void insert_resource(Resource **, Resource *);	/* Inserts an event in the  FES */
Resource *get_resource(Resource **);		/* Returns the first event from the FES */
Resource *new_resource(void );		/* Returns a new event */
void release_resource(Resource *);		/* Releases an event */
void delete_resource(Resource **,Resource *); /* Delete a resource */
Resource *print_list(Resource *, char *, short);
