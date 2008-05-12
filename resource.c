#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "resource.h"


Resource *free_resources = NULL;  /* Pointer to my free_list of events */

/* 
**  Function:    void delete_event(Event **last,Event *event)
**  Parameters:  Event **last  - Reference pointer for the list (the last
**                               element)
**               Event  *elem  - Record of type Event to be removed
**  Return:      none
**  Description: Remove the event 'elem' from the FES, leaving it in a
**               correct state. The event must already be in the FES and it is
**               not looked up. 
*/

void delete_resource(Resource **last,Resource *elem)
{

  if (elem==NULL) return;
  if ((*last)==NULL) return;

  if ((*last)->next==(*last))
   {
     /* There is a single event in the FES */
     if ( (*last)==elem)
      {
        (*last)=NULL;
      }
   }
  else
   {
     (elem->next)->prev = elem->prev;
     (elem->prev)->next = elem->next;
     if ((*last)==elem)
      {
         /* elem was the last event in the FES: update the FES reference */
        (*last) = elem->prev;
      }
   }

  /* Detach the record from the FES */
  elem->next = NULL;
  elem->prev = NULL;
  
  return;
}


Resource* delete_resource_POP(Resource **last,Resource *elem)
{

  if (elem==NULL) return;
  if ((*last)==NULL) return;

  if ((*last)->next==(*last))
   {
     /* There is a single event in the FES */
     if ((*last)==elem)
      {
        (*last)=NULL;
      }
   }
  else
   {
     (elem->next)->prev = elem->prev;
     (elem->prev)->next = elem->next;
     if ((*last)==elem)
      {
         /* elem was the last event in the FES: update the FES reference */
        (*last) = elem->prev;
      }
   }

  /* Detach the record from the FES */
  elem->next = NULL;
  elem->prev = NULL;
  
  return elem;
}

/* 
**  Function:    void insert_event(Event **last, Event *elem)
**  Parameters:  Event **last  - Reference pointer for the list (the last 
**                               element)
**               Event  *elem  - Record of type Event to be inserted
**  Return:      none
**  Description: Inserts 'elem' in the Event List referenced by **last
*/
void insert_resource(Resource **last, Resource *elem)
{
  Resource *p;
  if (elem==NULL) return;       /* Nothing to do */

  /* The Event List is empty: elem becomes the Event List */
  if ((*last)==NULL)
    {
       (*last) = elem;
       (*last)->next = elem;
       (*last)->prev = elem;
       return;
    }

  /* 
  ** elem is scheduled later than the last event in the list:
  ** it is inserted at the end and becomes the new last
  */
  //if (elem->time >= (*last)->time)
  if ( strcmp(elem->name,(*last)->name)>=0 )
    {
       elem->prev = (*last);
       elem->next = (*last)->next;
       ((*last)->next)->prev = elem;
       (*last)->next = elem;
       (*last) = elem;
    }
  else
  /* 
  ** lookup the correct position for elem, starting from the end
  ** of the list, i.e. following the prev pointer
  */
    {
       p = (*last);
       //while (elem->time < p->time && p->prev!=(*last))
       while ( (strcmp(elem->name,p->name) < 0)  && p->prev!=(*last))
         p = p->prev;
	   //if (elem->time < p->time)
       if (strcmp(elem->name,p->name) < 0)
         { 
		   /* p is the first element scheduled after elem: 
		      insert elem immediately before it*/
           elem->prev = p->prev;
           elem->next = p;
           (p->prev)->next = elem;
           p->prev = elem;
         }
       else
         {
		   /* elem must be the first event in the list:
		      insert elem after p (that is *last) */
           elem->prev = p;
           elem->next = p->next;
           (p->next)->prev = elem;
           p->next = elem;
         }
    }
  return;
}

/* 
**  Function:    Event *get_event(Event **last)
**  Parameters:  Event **last  - Reference pointer for the list (the last 
**                               element)
**  Return:      Pointer to the extracted event 
**  Description: Returns the next scheduled event, removing it from the top of
**               the FES. Returns NULL if the FES was empty
*/
Resource *get_resource(Resource **last)
{
  Resource *p;
  if ((*last)==NULL) return NULL;   /* The Event List is empty */
  if ((*last)->next==(*last))
    {
	  /* I'm removing the only element from the Event List */
      p = (*last);
      (*last) = NULL;
    }
  else
    {
	  /* Extract the first element (the successor of last) */
      p = (*last)->next;
      (p->next)->prev = (*last);
      (*last)->next = p->next;
    }
  return p;
}

/* 
**  Function:	 Event *new_event(void)
**  Parameters:  none
**  Return:      Pointer to a new event
**  Description: Returns a new event either allocating it or extracting it 
**               from the Free List
**  Side Effect: Extracts elements from the global Free List
*/
Resource *new_resource(void)
{
 extern Resource *free_resources;
 Resource *p;
 
 if (free_resources==NULL)
  {
    if ((p=(Resource *)malloc(sizeof(Resource)))==NULL)
      { printf("Error  : Memory allocation error\n");
        exit(1);
      }
    //p->time = (Time)0;
    p->name = "";
    insert_resource(&free_resources,p);
  }
 return(get_resource(&free_resources));  /* Returns the first element of the
                                      Free List */
}

/* 
**  Function:	 void release_event(Event *elem)
**  Parameters:  Event *elem - Event to be released
**  Return:      None
**  Description: Releases the record of the event, inserting it in the
**               Free List
**  Side Effect: Inserts elements in the global Free List
*/
void release_resource(Resource *elem)
{
 extern Resource *free_resources;
 //elem->time = (Time)MAX_TIME;   
 /* In this way the record is at the bottom of the Free List */ 
 strcpy(elem->name,"zzzzz");        
 insert_resource(&free_resources,elem);
}



void
	print_list(Resource *res_list)
	{
		Resource *last, *first;
		struct timespec t;
		//TODO prova malloc
		char *c;
		
		printf("\n\n***************************** List ***********************************\n\n");
		last= res_list;
		first=res_list->next;
		//res_list=res_list->next;
		
		
		if(res_list->next != NULL)
		{
			res_list=res_list->next;
			
			while(res_list->next!=first){
				
				
				t = res_list->status.st_mtim;
				
				//TODO print the std values,date(last modification) time (last modification) <DIR>(or not) <size in bytes> (if file) file name 
				//printf("%d %s\n",t.mday, res_list->name);
				printf("%s\n",res_list->name);
				res_list=res_list->next;
				
			}
			
			printf("%s\n",res_list->name);
			res_list=last;
		}
		//c=getchar();
		return;
	}
