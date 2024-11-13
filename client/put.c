/*
MEERA SUSAN TOMMY
PROJECT : TFTP
OBJECTIVE: upload file from client to server
FUNCTIONS: 
1: put()
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
#include"client.h"
/*uploading file from client to sever*/
int put(int sock_fd, struct sockaddr_in serv_addr,char *f_name)
{
	int fd = open(f_name,O_RDONLY);//file oppend in read only mode
//	printf("reached here\n");
	if(fd == -1) //in case file does not exist in the client directory
	{
		printf("\nINFO: File not found");
		return 0;
	}

	/*File exits -> send request to server*/
	printf("\nINFO: File : %s",f_name);
	printf("\nINFO: Transfer Mode: netascii");

	ini_req_pack *first = malloc(sizeof(ini_req_pack)); //object for request package => containing filename,WRQ,MODE
	establish_initial_request_package(f_name,&first,WRQ);  //initializing members of struct ini_req_pack

	/*sending request to upload a file*/
	int c_size = sendto(sock_fd,first,sizeof(*first),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
//printf("\nwrq request send\n");	
	if(c_size<0)     // ............to be commented this if and else
	{
	//	printf("\n WRQ message sent to server successfully; please che");
		printf("\nERROR: Try again, request msg not send to server");
	}
//	else
//		printf("\n error: WRQ message not send");

	//declaring objects for ack and data transfer structures
	ack_pack ack ;   //for recieving acknoledgement
	data_pack data;  //for sending data
	int block_no = 1;//nth time the data packet is going to be send ->represented by block_no

	struct timeval timeout;  //declaring and initializing timeout structure to retransmit data 
	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;
//	setsockopt( sock_fd,SOL_SOCKET,SO_RCVTIMEO, &timeout,sizeof(timeout));// .....comment this if not working

	char buffer[MAX_LEN];
	int resend;//to keep track of no of time data is been resend
//	printf("here1\n");

	while(1)
	{
//	printf("here2\n");
		resend = 0;
		while(resend < LIMIT)//tries to retrive the msg till resend reaches the limit ack / err
		{
//	printf("here3\n");
			int k = recvfrom(sock_fd,&ack,sizeof(ack),0,NULL,NULL);//recieve ack
	//		printf("inside ack loop\n");
			if(k>=0)//if ack recieved
			{
				unsigned short opcode = ntohs(ack.opcode);//fetching the first 2 bytes from buffer -> to check if recieved msg is an error or ack
		//		printf("opcode recived from the server %d\n",opcode);
		//		printf("opcode recived from the server %d\n",ntohs(ack.block_no));
				if(opcode == ERR)
				{
					printf("\nERROR: File already exists");
					close(fd);//closing the opend file
					return 1;
				}
				else if(opcode == ACK)
				{
				//	ack = (ack_pack *)buffer;//changing the string buffer to struct ack
					if(ntohs(ack.block_no) == block_no -1) //if we received the correct ack
						break;
					else 
						resend++;
				}
			//	printf("recieved ack\n");
			}
			else // ack not recieved
				resend++;//loop runs agin
		}
	//	printf("\n ack %d recieved\n",ntohs(ack.block_no));//.......to be commented
		if(resend == LIMIT)
		{
			printf("\nERROR: Couldn't recieve acknoledgement...server not resonding");
			break;
		}

		int n_bytes = read(fd,data.data,DATA_SIZE);//reading 512 bytes from file to data block in struct data
		if(n_bytes > 0)//contents are there in file
		{
			//initializing the data pack structure to send to server
			data.opcode = htons(DATA);
			data.block_no = htons(block_no);
			sendto(sock_fd,&data,sizeof(data),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));	//		printf("data block %d send\n",block_no);//...........to be comented
			block_no++;
			memset(data.data,0,sizeof(data.data));
		}
		else//it reached EOF
			break;
	}
	printf("\n########################");
	printf("\nINFO: File uploaded");
	free(first);
//	free(data);
//	free(ack);
	close(fd);
	return 0;
}

/*	printf("\n opcode %d",(first)->opcode);
	printf("\n filename- %s",first->filename);
	printf("\n byte_0%c",first->byte_0);
	printf("\n mode-  %s",first->mode);
	printf("\n byte_0%c",first->byte_end);*/
