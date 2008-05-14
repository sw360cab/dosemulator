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



/*
 * 
 * * 
 * TODO: wrapper for both cd and chdir
 * TODO: menage duoble quotes in the path ""
*/
 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/*
	 **  Function : void get_input(char *format,void *variable)
	 **  Return   : None
	 **  Remarks  : To be used instead of scanf. 'format' is a scanf format,
	 **             'variable' the pointer to the variable in which the input
	 **             is written.
	 */

void get_input(char *format,void *variable)
{
		static char linebuffer[255];
		char *pin;
	
		fgets(linebuffer, 255, stdin);	/*  Gets a data from stdin without  */
		pin = strrchr (linebuffer, '\n');	/*  flushing problems		    */
		if (pin!=NULL) *pin = '\0';
	
		sscanf(linebuffer,format,variable);	/* Read only the beginning of the   */
		/* line, letting the user to write  */
		/* all the garbage he prefer on the */
		/* input line.			    */
}
/*
int main(int argc,char **argv) 
   { 
		if (argc < 2)
		{  printf("Usage: %s <pathname>\n",argv[0]);
						 exit(1);
		}
				if (chdir(argv[1]) != 0)
						{ printf("Error in chdir\n");
								 exit(1);
						}
				
				system("pwd");
				pause();
	}
*/
