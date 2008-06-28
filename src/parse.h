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
#define BUF_MAX 512
#define MAXPATH 4096
#define N 20
#define TRUE 1
#define FALSE 0
#define STRING_LENGTH 1024

typedef struct elem param;

struct elem
{
	char *name;
	int type; // O -> path / file_name || 1 -> options
	param *next;
};

// pipe for current_dir
int current_dir[2];
// pipe for piped commands
int pipe_comm[2];

// check first string character to identify an option
// *** receive pased string - return 1 -> Yes / 0 -> No ***
int jolly_char(char *);

// insert a new element in a custom list of type *param
// *** receive list by reference , element to insert in lis *** 
void insert_e(param **, param *);

// create a new element of type *param
// *** return the new allocated element ***
param * new_elem();

// '-' and '_' are allowed for dir --- '.' and '/' supposed allowed for files
// space not handled
// *** receive path name, 0-> dir 1->file - return TRUE -> Ok / FALSE -> Forbidden ***
int under_s(char, int);

// check for forbidden characters - in particular name cannot start with digit or '_' or '-'
// *** receive path name, character to store errors by reference,  0-> dir 1->file ***
// *** return TRUE -> Ok / FALSE -> Forbidden ***
int alpha_num(char *, char *, int);

// function in charge of handling redirection
// return file pointer in case of success
// *** receive path name, length of name - return file descriptor to use ***
int redirector(char *, int);

// create supporting directories if a dest file
// for copy or redirectorie is specified with not existing dir
// ex -> a/b/c/d.txt
// *** receive path - return TRUE -> created / FALSE -> not a file ***
int create_dest_path (char *);

// return the complete string following a pipe "|" character
// *** receive a line to be parsed - return the string following a pipe ***
char* pipe_string(char *);

/*
 * parse a line passed as argument to identify all
 * the strings that are options of a command
 * *** receive a line to be parsed, a file descriptor by reference, a flag for pipe by reference ***
 * *** return list with paths and options ***  
 */
param* parse_options(char *, int *, int *);

/*
 * parse a line passed as argument to identify first string
 * as command and the rest as options
 * *** receive a line to be parsed, a string for command by reference, a string for options by reference ***
 */
void parse_line(char **, char **, char *);

// wrap for malloc
// *** receive the size of memory to allocate - return the reference to memory allocated ***
char *salloc (unsigned);

// wrap for realloc
// *** receive the reference to memory allocated, the size of memory to allocate *** 
// *** return the reference to memory reallocated ***
char *srealloc(char *, unsigned);

// dynamically get a line till return is pressed
// *** return read line ***
char *get_line();

// wrap of free to fre memory allocated for a list
void my_free(param**);

// checks if directory is empty --> return TRUE
// *** receive path name ***
int empty_dir (char *;


extern char *cd(char*,param*);
extern void attrib(param*);
extern void cls(param*);
extern void cp(param*);
extern void xcp(param*);
extern void disk_copy(param*);
extern void date(param*);
extern void del(param*);
extern void deltree(param*);
extern void rd(param*);
extern void dir(param*);
extern void echo(param*);
extern void fc(param*);
extern void find(param*);
extern void help(param*);
extern void list(param*);
extern void md(param*);
extern void more(param*);
extern int launch_exe(char*,param*);
