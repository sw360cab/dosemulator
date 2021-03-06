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

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include "parse.h"

/* check correcteness of the date */
// *** receive day, month, year ***
int check_date(int, int, int);

/*shows the current date and ask for a new date*/
void my_date();

/* check for paramters and launch my_date function */
// *** receive list with path and options ***
void date(param *);


short t_option= FALSE;

/* check correcteness of the date */
int check_date(int dd, int mm, int yyyy) {

	//general constraints
	if (dd<0 || dd>31 || mm<0 || mm>12 || yyyy<0) {

		fprintf(stderr, "Not valid date\n");
		return -1;
	}
	//sept,apr,jun,nov have 30 days
	if (mm==9 || mm==4 || mm==6 || mm==11) {
		if (dd>30) {
			fprintf(stderr,"Error: month %d has only 30 days\n",mm);
			return -1;
		}
	}
	//if feb, only years whose y%4=0 have 29 days, otherwise 28
	if (mm==2) {
		if (yyyy%4==0) {

			if (dd>29) {
				fprintf(stderr,"In year %d february month can have only 29 days",dd);
				return -1;
			} else {
				if (dd>28) {
					fprintf(stderr,"In year %d february month can have only 28 days",dd);
					return -1;
				}
			}

		}
	}

	return 0;
}

/*shows the current date and ask for a new date*/
void my_date() {

	time_t rawtime;
	struct tm * timeinfo;
	char *current_time_string = (char *)malloc(26);
	short flag=FALSE;
	int dd, mm, yyyy,n;
	char *line;
	char *new_time = (char *)malloc(strlen("\"20040908 +01:22\""));

	time(&rawtime);
	timeinfo = localtime( &rawtime);

	//time_ms = time(&time_accesible);
	//current_time_string = ctime(&rawtime);//it was &time_ms
	sprintf(current_time_string,"Current date: %d/%02d/%4d\n",timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900);
	fprintf(stdout,"%s\n", current_time_string);

	if (t_option == FALSE) {//if t option only show date and don't ask for new date,

		//TODO problem in while scanf: if i don't meet a number, 3 is returned
		while (flag==FALSE) {

			fprintf(stdout,"Please specify a new date: (dd/mm/yyyy):");
			line = get_line();

			if(strcmp(line,"quit")==0)
				exit(1);

			n = sscanf(line, "%d/%d/%d", &dd, &mm, &yyyy);

			if (n!=3){
				fprintf(stderr, "Not valid date\n");
				continue;
			}
			else {
				if (check_date(dd, mm, yyyy)==0) {

					/*
					time( &rawtime);
					timeinfo = localtime( &rawtime);
					timeinfo->tm_year = yyyy;
					timeinfo->tm_mon = mm;
					timeinfo->tm_mday = dd;
					 */
					//					fprintf(stdout,"setted: d:%d m:%d y:%d\n",timeinfo->tm_mday,timeinfo->tm_mon ,
					//					timeinfo->tm_year);
					//					fprintf(stdout,"(asctime)time setted %s - ,\n", ctime(mktime(timeinfo)),asctime(timeinfo));

					//September 8, 2004 01:22  --> date --set="20040908 01:22";

					if(dd<10 && mm<10){

						sprintf(new_time,"date --set=%d%d%d%d%d",yyyy,0,mm,0,dd);


					}
					else if(dd<10 && mm>9){

						sprintf(new_time,"date --set=%d%d%d%d",yyyy,mm,0,dd);

					}
					else if(dd>9 && mm<10){

						sprintf(new_time,"date --set=%d%d%d%d",yyyy,0,mm,dd);

					}
					else{
						sprintf(new_time,"date --set=%d%d%d",yyyy,mm,dd);
					}

					system(new_time);
					flag = TRUE;

				}
			}
		}

		//“date 0722071500”. This will display the date as Sat July 22, 07:15 2000. 

	}
	//can't free current_time_string
	//free(current_time_string);
	free(new_time);

}

/* check for paramters and launch my_date function */
void date(param *parameters) {

	param *iterator = parameters;


	if (iterator == NULL) {

		my_date();


	}

	else {
		while (iterator != NULL) {

			if (iterator->type==0) {
				if (strlen(iterator->name)>0) {
					fprintf(stderr,"Wrong command syntax\n");
					return;
				} else
					my_date();

			} else {

				if (strcasecmp(iterator->name, "\\T")==0) {
					t_option = TRUE;
					my_date();
				} else {
					fprintf(stderr,"DATE: not valid argument\n");
					return ;
				}

			}

			iterator=iterator->next;
		}
	}

}

