#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXPATH 4096

void erase(char *pth)
	{
	if ( unlink(pth) == -1)
	   	{
	   		fprintf(stderr, "Unable to delete file %s\n",pth);
	   		exit(1);
	   	}
	printf("file deleted %s\n", pth);
	return;
	}

void recur(char *current_path)
{
	DIR *dp;
	struct dirent *ep;
	char temp_path[MAXPATH];
	char file_name[MAXPATH];
	
	dp = opendir(current_path);

	if (dp != NULL) {
		while ( (ep = readdir(dp)) )
			{
			//ep->type: 4 dir, 8 file
			if ( ep->d_type==4) {		// is a directory get into recursively
				strcpy(temp_path,current_path);
				//if the current dir doesn't finish with '/',
				//I should add it before appending the currente resource name
				if ( current_path[strlen(current_path)-1] != '/')
								strcat(temp_path, "/");
								
				strcat(temp_path, ep->d_name);
				
				recur(temp_path);
			}
			
			else
				{
				if ( (strcmp(ep->d_name, ".") != 0 ) && (strcmp(ep->d_name, "..")!= 0))
					{
					strcpy(file_name,current_path);
					
					if ( current_path[strlen(current_path)-1] != '/')
						strcat(file_name, "/");
													
					strcat(file_name, ep->d_name);
					
					erase(file_name);
					}
				}
			}
			//free(temp_path);
			closedir(dp);
			return;
		}
}

int main(int argc, char **argv)
	{
	
	char nonno[50];
	char current_dir[MAXPATH];
	
	strcpy(nonno,"mookookokooko");
	printf ("nonno is %s\n", nonno);
	strcpy(nonno,"culo");
	printf ("nonno is %s\n", nonno);
	
	strcpy(current_dir,argv[1]);
	recur(current_dir);
	exit(0);
	}
