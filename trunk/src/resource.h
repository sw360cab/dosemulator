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
	Resource *next;	/* Next resource in the list */
	Resource *prev; /* Previous resource in the list */
	char *name;     /* name of the resource */
	char *path;     /*path of the resource*/
	unsigned char type; /* Type of resource, 4 =  directory , 8 = file */
	struct stat status; /* The result of the fstatus */
	short flag; 		/* a generic flag*/
	//char  *record; 	/* Generic pointer parameter, to be used  through a cast to the specific type   */
	//Record *record;
};

/* Prototypes */

/* Inserts a resource 
 * ***receive a pointer to a list, the resource pointer element to be inserted*/
void insert_resource(Resource **, Resource *);

/* Inserts a resource ordered,
 * ***receive a pointer to a list, the resource pointer element to be inserted*/
void insert_resource_order_by_type(Resource **, Resource *);

/* Returns the first resource 
 * ***recevive a pointer to a list, ***return the first resource*/
Resource *get_resource(Resource **);		

/* pop an element from list,
 * ***receive a pointer to a list,the resource pointer to pop, ***return the resource extracted */
Resource* delete_resource_POP(Resource **, Resource *); 

/* ***Returns a new resource */
Resource *new_resource(void );		

/* Releases a resource 
 * receive a resource pointer*/
void release_resource(Resource *);		

/* Delete a resource ,
 * ***receive a pointer to list of resources, the resource pointer to delete*/
void delete_resource(Resource **,Resource *); 

/* print a list of resources - the third argument is a generic pointer parameter,
 * to be used  through a cast to the specific type ;
 * ***receive the first element pointer of the list, the path of the father dir,and options
 * ***return list of dirs contained in path */
Resource *print_list(Resource *, char *,char *); 

/* create a resource with parameters;
 * ***receive the stat, the name, the type and the path of the resource father folder
 * ***return the created resource * */
Resource *create_res(struct stat, char[], unsigned char,char *);

/* build the absolute path of the resource;
 * ***receive the resource name and owner dir path, 
 * ***return the absolute path of the resource*/
char *build_path(char *,char *); 

/* tells wheter resource is read only;
 * ***receive the mode (casted to long), ***return 0 if resource isn't read only, 1 otherwise*/
int is_read_only(long);

/* given a string , it extracts double quotes from beginning and end
 * ***receive a string, ***return the string without double quotes*/
char *extract_double_quotes(char*); 

/* converts a decimal number into a binary one as array of chars 
 * ***receive a long, and a pointer to an array of characters*/
void dec2bin(long, char*);


/* It reads the time from ts and puts it in buffer (of size len) as a string 
 * ***receive the structure timespec, a char array where to save the string,the length of the array
 * */
void timespec2string(struct timespec *, char[], int);

/* standard resource info printing ;
 * ***receive a resource pointer, pointers to, respectively, number of dirs,files and file size counters  
 * */
void stdprint(Resource *, int *, int *, int *) ;

/* time info print of folder . and .. ;
 * ***receive the structure stat of the folder, an int flag to indicate wheter it's the father folder(..) or not 
 * */
void strdprint_time_parents(struct stat, int);

/* folder . and .. info print ;
 * ***receive the path of the folder;
 * */
void stdprint_parents(char *path);
