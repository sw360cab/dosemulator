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
int jolly_char(char *);
// insert a new element in a custom list of type *param
void insert_e(param **, param *);
// create a new element of type *param
param * new_elem();
// '-' and '_' are allowed for paths --- '.' and '/' supposed allowed for files
int under_s(char, int);
// check for forbidden characters - in particular name cannot start with digit or '_' or '-'
int alpha_num(char *, char *, int);
// function in charge of handling redirection
// return file pointer in case of success
int redirector(char *, int);
// return the complete string following a pipe "|" character
char* pipe_string(char *);
/*
 * parse a line passed as argument to identify all
 * the strings that are options of a command
 */
param* parse_options(char *, int *, int *);

/*
 * parse a line passed as argument to identify first string
 * as command and the rest as options
 */
void parse_line(char **, char **, char *);
// wrap for malloc
char *salloc (unsigned);
// wrap for realloc
char *srealloc(char *, unsigned);
// dynamically get a line till return is pressed
char *get_line();
// wrap of free to fre memory allocated for a list
void my_free(param**);

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
