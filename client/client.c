/*
   CLIENT CODE
###################################################################################################
MEERA SUSAN TOMMY
.................

			Descriptions   : Client side of tftp protocol
            Objective      : To send/recive file to/from server
            Usage          : ./client <server ip> <port> //<filename> <output filename> [get|put]
			Functions      : main()
###################################################################################################
   */
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include"client.h"

int main(int argc,char *argv[])
{
	char user_comd[50];
	printf("Type help to get supported features");

	if(argc == 2)
	{
		connect_to_server(argv[1]);
	}

	while(1)
	{
		printf("\ntftp]\t");
		scanf("%[^\n]",user_comd);

		//check if i/p is help / connect / bye ;; anything else then error command
		if(strcmp(user_comd,"help")==0)
			help();
		else if(strstr(user_comd,"connect"))
		{
			char *token = strtok(user_comd," ");
			token = strtok(NULL," ");
	//		printf("\nServer IP : %s",token);
			connect_to_server(token);
		}
		else if(strcmp(user_comd,"bye")==0 || strcmp(user_comd,"quit")==0) // exits
		{
			return 0;
		}
		else       //error
		{
			printf("\nERROR: Not connected to server");
		}
		__fpurge(stdin);

	}
//1turn 0;
}
