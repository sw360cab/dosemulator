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

int jolly_char(char *);
void insert_e(param **, param *);
param * new_elem();
int under_s(char, int);
int alpha_num(char *, char *, int);
int redirector(char *, int);
char* pipe_string(char *);
param* parse_options(char *, int *, int *);
char* get_line();

extern void cp(param*);
extern void xcp(param*);
extern void disk_copy(param*);
extern void md(param*);
extern void del(param*);
extern void deltree(param*);
extern void rd(param*);
extern void list(param*);
extern void help(param*);
extern char *cd(char*,param**);
extern void echo(param*);
