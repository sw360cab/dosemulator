#define MAXPATH 4096
#define FALSE 0
#define TRUE 1
typedef struct elem param;

struct elem
{
	char *name;
	int type; // O -> path / file_name || 1 -> options
	param *next;
};

int jolly_char(char *line);
void insert_e(param **p_list, param *p);
param * new_elem();
param* parse_options(char *opt);
