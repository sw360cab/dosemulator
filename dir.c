//TODO recursion algorithm
//TODO printout formatting functions
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>  
#include <dirent.h>
#include "resource.h"
//TODO better definition of maximum path length
#define MAXPATH 350
/* 
 * D  Directory, R  File sola lettura, H  File nascosti
 * A  File archivio, S  File di sistema, - Prefisso per negare l'attributo
 * 
 */
//deafult views /A option

short show_directory;
short show_read_only;
short show_hidden_files;
short show_system_files;
short show_archive_files;

short show_owner; // /Q option 
short show_recursive; // /S option 

Resource *resources_list;

//it initialize tha mask with default options
void initialize(){
	 	 
	 show_directory = 1;
     show_read_only = 0; 
     show_hidden_files = 0;
	 show_system_files = 0; //TODO do "system" files exist under linux ?
	 show_archive_files = 1;//TODO do archive files exist under linux ?

	 show_owner = 0; // /Q option 
	 show_recursive = 0; // /S option
	 resources_list = NULL;
	 
}

Resource *create_res(struct stat status, char *res_name){
		
		Resource *res;
		
		res = new_resource();
		res->status = status;
		res->name = res_name;
		return res;
		
}

int
main (int argc, char **argv)
{
  
  DIR *dp;
  Resource *res;
  struct dirent *ep;
  struct stat status;
  char current_dir[MAXPATH], temp_path[MAXPATH];
  char process_node='1'; //tell us wheter i have to process(=visualize) the node or not
  int *p;
  off_t size;
  
  
  if(argc != 2){
	  printf("Please specify one argument\n");
	  return -1;
  }
  
  initialize();
  
  strcpy(current_dir,argv[1]);
  
  dp = opendir (current_dir);
  if (dp != NULL)
    {
      while (ep = readdir (dp)){
    	 
    	  
    	  //if ep = . or .. , i don't need to process the node
    	  if( (strcmp(ep->d_name,".") == 0 ) || ( strcmp(ep->d_name,"..") == 0) ){
    	     		  printf("%s\n",ep->d_name);
    	     		  continue;
    	  }
    	  if(show_hidden_files==0 && ep->d_name[0]=='.'){
    		  continue;
    	  }
    	  //ep->type: 4 dir, 8 file
    	  if(show_directory==0 && ep->d_type==4){
    		  continue;
    	  }
    	  //at every round, i reset the temp_path to current dir value
    	  strcpy(temp_path,current_dir);
    	    	  
    	  //if the current dir doesn't finish with '/',i should add it before appending the currente resource name
    	  if( ((char)current_dir[strlen(current_dir)-1]) != '/'){
    		  
    		  strcat(temp_path,"/");
    	  		      	  
    	  }
       	  
    		  strcat(temp_path,ep->d_name);
    	      	     	  
    	  //TODO try to understand why files that terminates with "~" (temporary files?) aren't acessible by fstat
    	  //files that terminates with "~" (temporary files?) aren't acessible by fstat
    	  //TODO decide wheter files that terminates with "~" should be added to the list
    	  if(temp_path[strlen(temp_path)-1] == '~'){
    		  printf("%s\n",ep->d_name);
    		  continue;
    	  }
    	  
    	  if( (p=open(temp_path,O_EXCL)) == NULL){
    		  
    		  printf("Invalid path : %s\n",temp_path);
    		  return -1;
    	  }
    	  
    	  //fstat: these functions returns information about a file. No permissions are required on the file itself, 
    	  //but -- in the case of stat() and lstat() -- execute (search) permission is required on all of the 
    	  //directories in path that lead to the file.
    	  //TODO privilege problems : print "ls: cannot open directory /home/lost+found/: Permission denied"

    	  if(fstat(p,&status) != 0){
		    		      			  
		    			  printf("Can't retrieve information from resource %s\n", temp_path);
		    			  //return -1;
		    			  continue;
		    		    		
		  }
    	  //st.mode 400 (user read only)-> 33024
    	  //TODO should i menage other read only cases ?
    	  
    	  if( show_read_only==1 && (((unsigned short)status.st_mode) == 33024) )
    		  continue;
    	  
    	  res = create_res(status,ep->d_name);
		  insert_resource(&resources_list,res);
		  
      	  
    	  printf("%s \t %lu\n", ep->d_name,(unsigned long)status.st_size);
    	  
    	  
    	  
      }
      (void) closedir (dp);
    }
  else
    printf("Couldn't open the directory.\n");

  
  print_list(resources_list);
  return 0;
}

