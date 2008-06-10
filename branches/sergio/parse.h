#define BUF_MAX 512
#define MAXPATH 4096
#define N 20
#define TRUE 1
#define FALSE 0

typedef struct elem param;

struct elem
{
	char *name;
	int type; // O -> path / file_name || 1 -> options || 2 -> redirezione
	param *next;
};

// pipe
int current_dir[2];

int jolly_char(char *line);
void insert_e(param **p_list, param *p);
param * new_elem();
param* parse_options(char *opt);
char* get_line();

extern void cp(param**);
extern void md(param*);
extern void del(param*);
extern void deltree(param*);
extern void list(param*);
extern char *cd(char*,param**);