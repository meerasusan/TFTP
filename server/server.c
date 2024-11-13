/*
   SERVER CODE
######################################################################
MEERA SUSAN TOMMY
.................

Descriptions   : Server side of tftp protocol
 *           Objective      : Ready to recive/send data to client 
 *           Usage          : ./server <port number>
 			function       : main()
######################################################################
 */
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include"server.h"

//#define SERVER_BUFF 512

int main()
{

	int sock_fd, cli_len;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	cli_len = sizeof(cli_addr);

	if((sock_fd = socket(AF_INET,SOCK_DGRAM,0))<0)//creating the socket
	{
		perror("\nSocket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	memset(&cli_addr,0,sizeof(cli_addr));

	/*initialising server address*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)//binding the server and checking error
	{
		perror("bind");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	/*initialising and setting sets for read_fd for select()*/
	fd_set read_fd_set;
//	FD_ZERO(&read_fd_set);
//	FD_ZERO(&active_fd_set);
//	FD_SET(sock_fd,&read_fd_set);
	int backup_fd;

	char buffer[MAX_DATA];//to store the data recived from the client
	char fname[WORD_COUNT];//to store the file name

	printf("\nServer is waiting......\n");
	while(1)
	{
//		printf("%ld\n",sizeof(buffer));
		memset(buffer,0,sizeof(buffer));//clearing the buffer
//		printf("h\n");
	//	read_fd_set = active_fd_set;//re-ini`:tializing read again
	FD_ZERO(&read_fd_set);
	FD_SET(sock_fd,&read_fd_set); //re-initializing read_fd
	backup_fd = sock_fd;

		if(select(backup_fd+1,&read_fd_set,NULL,NULL,NULL)<0)//calling sellect andchecking errors
		{
			perror("select");
			close(sock_fd);
			exit(EXIT_FAILURE);
		}
		if(FD_ISSET(sock_fd,&read_fd_set))//to see if any data was recieved at sock_fd
		{
			//if data recieved at sock_fd
	int	k = recvfrom(sock_fd,buffer,sizeof(buffer),0,(struct sockaddr *)&cli_addr,&cli_len);
		if(k<0)
			{
				perror("recvfrom");
				continue;
			}

			unsigned short opcode = ntohs(*(unsigned short *)buffer);//takes the first 2 bytes from buffer to check if it is a data pack/ack pack/wrq_rrq pack/err pack
	//printf("\nrecieved a pack %d\n",opcode);
			switch(opcode)
			{
				case RRQ:
					define_rrq(sock_fd,&cli_addr,cli_len,buffer,fname);
					break;
				case WRQ:
					define_wrq(sock_fd,&cli_addr,cli_len,buffer,fname);
					break;
				case DATA:
					define_data(sock_fd,&cli_addr,cli_len,buffer,fname);
					break;
		//case ACK:
			//define_ack(sock_fd,&cli_addr,buffer);
		//			break;
				case ERR:
					define_err(sock_fd,&cli_addr,cli_len,buffer,fname);
					break;
			}
		}

	}

close(sock_fd);
return 0;

}

