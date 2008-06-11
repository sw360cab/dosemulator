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

// pipe
int current_dir[2];

int jolly_char(char *);
void insert_e(param **, param *);
param * new_elem();
int under_s(int, int);
int alpha_num(char *, char *, int);
int redirector(char *, int);
param* parse_options(char *, int *);
char* get_line();

extern void cp(param*);
extern void md(param*);
extern void del(param*);
extern void deltree(param*);
extern void list(param*);
extern char *cd(char*,param**);
extern void echo(param*);
