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
	struct stat status; /* The result of the fstatus */
	short flag; /* a generic flag*/
	//char  *record; /* Generic pointer parameter, to be used  through a cast to the specific type   */
	//Record *record;
      };

/* Prototypes */
void insert_resource(Resource **, Resource *);	/* Inserts an event in the  FES */
void insert_resource_order_by_type(Resource **, Resource *);	/* Inserts an event in the  FES */
Resource *get_resource(Resource **);		/* Returns the first event from the FES */
Resource* delete_resource_POP(Resource **, Resource *);/* pop an element from list */ 
Resource *new_resource(void );		/* Returns a new event */
void release_resource(Resource *);		/* Releases an event */
void delete_resource(Resource **,Resource *); /* Delete a resource */
Resource *print_list(Resource *, char *,char *); /*print a list of resources - the third argument is a generic pointer parameter, to be used  through a cast to the specific type */
Resource *create_res(struct stat, char[], unsigned char,char *);/*create a resource with parameters*/
char *build_path(char *,char *); /* given the resource name and owner dir path, it returns the full path of the resource*/
int is_read_only(long);/*tells wheter resource is read only*/
char *extract_double_quotes(char*); /*given a string , it extracts double quotes from beginning and end*/
void dec2bin(long, char*);/*converts a decimal number into a binary one as array of chars */
char *build_path(char *, char* );/* build the full path of a resource, given its name and folder parent containing path */
void timespec2string(struct timespec *, char[], int);/* It reads the time from ts and puts it in buffer (of size len) as a string */
void stdprint(Resource *, int *, int *, int *) ;/* standard resource info printing */
void strdprint_time_parents(struct stat, int);/* time info print of folder . and .. */
void stdprint_parents(char *path);/* folder . and .. info print */

