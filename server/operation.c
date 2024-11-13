/*
   MEERA SUSAN TOMMY
PROJECT : TFTP
FUNCTION : 
1: define_data
2: define_wrq
3: define_rrq
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
/*recieves request to download file from client ; send data ; recives acknowledgement*/
void define_rrq(int sock_fd,struct sockaddr_in *cli_addr,int cli_len,char *buffer,char *fname)
{
//	printf("reached rrq\n");
	memset(fname,0,WORD_COUNT);//clearnig the current file name to store new file name that needs to downloaded
	ini_recv_pack *first = (ini_recv_pack *)buffer;// converts the buffer to ini_recv_pack
	strcpy(fname,first->fname);//extracts the new file name
	int fd = open(fname,O_RDONLY);//file opend in read onlu mode
	if(fd == -1)
	{
//		file not present in server
		err_pack err;
		err.opcode = htons(ERR);
		err.error_code = htons(3);
		sendto(sock_fd,&err,sizeof(err),0,(struct sockaddr *)cli_addr,cli_len);
		return;
	}
//	printf("file present in server\n");
	data_pack data;
	data.opcode = htons(DATA);
	unsigned short block_no = 1;
	int resend =1, n_bytes;
	while((n_bytes = read(fd,data.data,DATA_SIZE))>0)//read 512 bytes from the server file till EOF is reached
	{
		resend = 1;
			data.block_no = htons(block_no);//asigns the block no
			data.data[DATA_SIZE] = '\0';
		while(resend<LIMIT)
		{
	//		printf("%s",data.data);
			sendto(sock_fd,&data,4 + n_bytes,0,(struct sockaddr *)cli_addr,cli_len);//sends the data pack to client
	//		printf("data pack %d send\n",block_no);
			ack_pack ack;//wait for the ack from client
			int k = recvfrom(sock_fd,&ack,sizeof(ack),0,(struct sockaddr *)cli_addr,&cli_len);
			if(k>=0 && ntohs(ack.opcode) == ACK && ntohs(ack.block_no) == block_no)//check if data is recived from client and it is correct ack pack
			{
			//	printf("recievd ack %d\n",ntohs(ack.block_no));  //continue to read data 
				break;
			}
			else if(k>=0 && ntohs(ack.opcode) == ERR)//check if data is recived from client and it is err pack
			{
			//	printf("recved err pack from client");
				close(fd);//cloces the file
				return;
			}
			else
				resend++;
		}
		if(resend == LIMIT)
		{
	//		printf("retries exhausted\n");
			break;
		}
		block_no++;
			memset(data.data,0,sizeof(data.data));//clears the contents in data.data
	}
//	printf("file send\n");
	close(fd);
}

/*recieves request to upload file from client ; check if file already present in server; if not, sends acknowledgement*/
void define_wrq(int sock_fd,struct sockaddr_in *cli_addr,int cli_len,char *buffer,char *fname)
{
//	printf("reached wrq\n");
	memset(fname,0,WORD_COUNT);//clears the file name
	ini_recv_pack *first = (ini_recv_pack *)buffer;//initializes first with contents of buffer
	strcpy(fname,first->fname);//gets the file name 
	FILE *fptr = fopen(fname,"r");// to check if file is present in server side; 
	if(fptr) //if file is present send an messg to client that file is already present in server; uploading cancelled
	{
	//	printf("file present in server\n");
		err_pack err;
		err.opcode = htons(ERR);
		err.error_code = htons(2);
		sendto(sock_fd,&err,sizeof(err),0,(struct sockaddr *)cli_addr,cli_len);//err pack send
		fclose(fptr);
		return;
	}
//	printf("file not in server..\n");  file not present => sends ack with block no 0 to start uploadig
	ack_pack ack;
	ack.opcode = htons(ACK);
	ack.block_no = htons(0);
	sendto(sock_fd,&ack,sizeof(ack),0,(struct sockaddr *)cli_addr,cli_len);// ack send

//	printf("ack for wrq send to cli\n");

}

/*recieves a data pack from client;  and sends acknowledgment*/
void define_data(int sock_fd,struct sockaddr_in *cli_addr,int cli_len,char *buffer,char *fname)
{
	//	printf("reached data\n");
	FILE *fptr = fopen(fname,"a");//opens file in append mode
	if(!fptr)
	{
		//		printf("file cannot be opened in server\n");
		err_pack err;
		err.opcode = htons(ERR);
		err.error_code = htons(5);
		sendto(sock_fd,&err,sizeof(err),0,(struct sockaddr *)cli_addr,cli_len);
		//	fclose(fptr);
		return;
	}

	data_pack *data = (data_pack *)buffer;
	data->block_no = ntohs(data->block_no);
	//	printf("recieved data pack of %d\n",data->block_no);
	//	printf("%s\n",data->data);
	fwrite(data->data,1,strlen(data->data),fptr);//writes data to file
	//	printf("wrote data to file\n");
	fclose(fptr);//close the file

	ack_pack ack;//creats object to send ack pack to client
	ack.opcode = htons(ACK);
	ack.block_no = htons(data->block_no);
	sendto(sock_fd,&ack,sizeof(ack),0,(struct sockaddr *)cli_addr,cli_len);
	//	printf("ack %d send\n",data->block_no);
	//	printf("%d\n",MAX_DATA);
	memset(buffer,0,516);//reseet buffer
	//	printf("buffer -- %s\n",buffer);
}
void define_err(int sock_fd,struct sockaddr_in *cli_addr,int cli_len,char *buffer,char *fname)
{
	err_pack *err = (err_pack *)buffer;
	if(ntohs(err->error_code)== 5 )
		printf("\nERROR: recived error from client: Unable to open file\n");
}
