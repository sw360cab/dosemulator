#include <sys/stat.h>  

typedef struct resource_unit Resource;


struct resource_unit
      {
	Resource *next;	/* Next event in the list */
	Resource *prev;    /* Previous event in the list */
	char *name;     /* name of the resource */
	//int   type;     /* Type of event, to be defined */
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
void print_list(Resource *);
