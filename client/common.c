/*MEERA SUSAN TOMMY
PROJECT : TFTP
FUNCTIONS:
1: help()
2: connect_to_server()
3: establish_initial_request_package
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
/* display the help menu*/
void help()
{
	printf("USSAGE:\nconnect <server_ip>   :Connect to server\nget <file_name>    :Receive a file from server\nput <file_name>   :Send a file to server\nbye /quit   :Exit the application");
}

/* check for incomming command, differentiate it between -> get/put/bye */
int connect_to_server(char *server_ip)
{
	__fpurge(stdin);
	char command[30],f_name[20];
//	printf("\nserver addr = %s",server_ip); //......commented

	/* Establishing UDP connetion */
	int sock_fd,c_size;
	struct sockaddr_in serv_addr;

	if((sock_fd = socket(AF_INET,SOCK_DGRAM,0))<0)  //creating socket
	{
		printf("\nERROR: could not create socket");
		return 1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	serv_addr.sin_addr.s_addr = inet_addr(server_ip);

	printf("\nINFO: Connected to server");
//	printf("\nsock_fd = %d",sock_fd);//.....commented

	while(1)
	{
		printf("\ntftp]\t");
		memset(command,0,sizeof(command));//clearing or resetting the contents in command and file_name
		memset(f_name,0,sizeof(f_name));
		scanf("%[^\n]",command);
//		printf("command2 %s\n",command);
		getchar();
		char inst[6];
		sscanf(command,"%s",inst);//extracts the first word to check if its get/ put
//		printf("inst %s %d\n",inst,strlen(inst));
		if(strcmp(inst,"put")==0)  //upload to server
		{
		//	printf("\n recieved put cmd with fname- %s",strchr(command,' ')+1);//.......commented
			put(sock_fd,serv_addr,strchr(command,' ')+1);		//calling put function to upload data
		}
		else if(strcmp(inst,"get")==0)//download from server
		{
		//	printf("\n recieved get cmd with fname- %s",strchr(command,' ')+1);//........commented
			get(sock_fd,serv_addr,strchr(command,' ')+1);//calling get function to download data
		}
		else if(strcmp(command,"bye")==0 || strcmp(command,"quit")==0)//exit the programme
		{
			close(sock_fd);//close the socket
			exit(0);
		}
		else
		{
			printf("\nERROR: Invalid command");
		}
		memset(command,0,sizeof(command));
		memset(f_name,0,sizeof(f_name));
		__fpurge(stdin);
	}

//	close(sock_fd);
	return 0;
}

/* setting the initial request packet to the server for upload(wrq) / download(rrq) */
int establish_initial_request_package(char *f_name,ini_req_pack **first,int rrq_or_wrq)
{
	(*first)->opcode = htons(rrq_or_wrq);
	strcpy((*first)->filename,f_name);
	(*first)->byte_0 = '0';
	strcpy((*first)->mode,"netascii");	
	(*first)->byte_end = '0';
	return 0;
}
