/*
   MEERA SUSAN TOMMY
PROJECT: TFTP
OBJECTTIVE: perform downloading operation
FUNCTIONS:
		1: get()//perform downloading operation
		2: recv_data_pack
		3: file_open_and_write
		4: send_ack_pack
		5: send_err_pack
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
/* perform downloading operation*/
int get(int sock_fd, struct sockaddr_in serv_addr,char *fname)
{
	//	printf("in get");
	printf("\nINFO: File : %s",fname);
	printf("\nINFO: Transfer Mode: netascii");

	ini_req_pack *first = malloc(sizeof(ini_req_pack));
	establish_initial_request_package(fname,&first,RRQ);//initialize the members of struct ini_req_pack to send rrq request

	if(sendto(sock_fd,first,sizeof(*first),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	{                                                   //rrq not sent to server
		perror("sendto");
	}
	free(first);
	//rrq msg send to server
	struct timeval timeout;//for timeout funstion initializing structure 
	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;
	setsockopt(sock_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	int block_no = 1;//to check the correctness of data packet and send data pack
	char buffer[MAX_LEN];//to store the recieved packet, send by the server
	int resend;//variable to keep track of no of attempts to recieve data from server
	int recv_data_len;//variable to store the length of file contends recievd from server

	while(1)
	{
		resend =0;
		while(resend< LIMIT)
		{
			if(recvfrom(sock_fd,buffer,sizeof(buffer),0,NULL,NULL)>=0)
			{                               //recieved data  successfully
											//		printf("recievd pack\n");
				unsigned short opcode = ntohs(*(unsigned short*)buffer);//to check if it is a error pack or data packet
				if(opcode == 5)//error pack
				{
					//			printf("recieved error pack\n");
					//		printf("file not foud in sserver\n");
					err_pack *err = (err_pack *)buffer;//with the err_no identify whicj error
					if(ntohs(err->error_code) == 5)
						printf("\nERRER: File cannot be opened");
					else if(ntohs(err->error_code) == 3)
						printf("\nERRER: File not found in server");
					return 1;
				}
				else if(opcode == 3)//data pack
				{
					//		printf("\ngot data pack\n");
					if((recv_data_len = recv_data_pack(sock_fd,serv_addr,fname,block_no,buffer))==1)
						return 1;
					//		printf("back here  recv_data_len: %d\n",recv_data_len);
					block_no++;
					break;
				}
				else //if the recieved pack is something else
					resend++;
			}//failed to recieve the data from server
			else 
				resend++;
		}
		//		printf("\n916\n");
		if(resend == LIMIT)//on failing to retransmit the data 3 times
		{
			printf("ERROR: Retries Exhausted\n");
			return 1;
		}

		//		printf("back 5678\n");
		//		printf("back here  recv_data_len: %d DATA_SIZE %d\n",recv_data_len,DATA_SIZE);
		memset(buffer,0,MAX_LEN);//resetting the buffer
		if(recv_data_len < DATA_SIZE)//check if its the last data packet
		{
			//			printf("last pack\n");
			break;
		}
	}
	//completed the download
	printf("\n#########################");
	printf("\nINFO: File downloaded\n");
	return 0;
}

/*Extracting file data from the recievd buffer and writting it to the file*/
int recv_data_pack(int sock_fd,struct sockaddr_in serv_addr,char *fname,int block_no,char *buffer)
{
	data_pack *data = (data_pack *)buffer;  //storing the buffer data to data_pack elements
	if(ntohs(data->block_no) == block_no)//check if the recieved data pack is the correct one
	{
		//		printf("\nrecved data pack %d\n",ntohs(data->block_no));
			data->data[DATA_SIZE] = '\0';
		if(file_open_and_write(fname,data->data)==1)//calling function to open and write data to file
		{                                             
			printf("ERROR: File cannot be opend\n");
			send_err_pack(sock_fd,serv_addr);//sending error pack to server indicating that file cant be openned
			return 1;
		}
		//		printf("data written to file\n");
		//file was opened and data written successfull, so sending ack pack 
		send_ack_pack(sock_fd,serv_addr,block_no);//this func sends ack pack
	}
	//	printf("\n%d\n",1+strlen(data->data));
	return 1+strlen(data->data);//returns no of charactors written to the file + 1

}

/*Opening and writing data to the downloaded file*/
int file_open_and_write(char *fname,char *data)
{
	//	printf("file name%s\n",fname);
	//	printf("%s\n",data);

	FILE *fptr = fopen(fname,"a");
	if(!fptr)
		return 1;

	fwrite(data,1,strlen(data),fptr);
	//	printf("done\n");

	fclose(fptr);
}

/*Sending error packet to server*/
void send_err_pack(int sock_fd,struct sockaddr_in serv_addr)
{
	//	printf("inside send err pack func\n");
	err_pack err;
	err.opcode = htons(ERR);
	err.error_code = htons((unsigned short)5);
	sendto(sock_fd,&err,sizeof(err),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	//	printf("err pack send inside send err pack");
}

void send_ack_pack(int sock_fd,struct sockaddr_in serv_addr,int block_no)
{
	ack_pack ack;//send ack to server
	ack.opcode = htons(ACK);
	ack.block_no = htons(block_no);
	sendto(sock_fd,&ack,sizeof(ack),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	//	printf("ack %d send to server\n",block_no);
}
