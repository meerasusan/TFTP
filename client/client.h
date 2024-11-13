#ifndef CLIENT
#define CLIENT

#define WORD_COUNT 30
#define DATA_SIZE 512
#define MAX_LEN 516
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERR 5
#define TIMEOUT 10
#define LIMIT 3
#define SERVER_PORT 5000

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

typedef struct RRQ_WRQ_packet
{
	unsigned short opcode;// 1=> RRQ || 2=> WRQ
	char filename[WORD_COUNT];
	char byte_0;
	char mode[WORD_COUNT]; //netascii
	char byte_end;//0
}ini_req_pack;

typedef struct data_packet
{
	unsigned short opcode;// 3
	unsigned short block_no;// no of data packets send, 1st, 2nd, 3rd ets
	char data[DATA_SIZE];
}data_pack;

typedef struct ack_packet
{
	unsigned short opcode;  // 4
	unsigned short block_no; //no of ack packet send 1st, 2nd, 3rd
}ack_pack;

typedef struct error_packet
{
	unsigned short opcode;  // 5
	unsigned short error_code;  
//	char err_msg[WORD_COUNT];
//	char byte_0;
}err_pack;

void help();
int connect_to_server(char *);
int put(int, struct sockaddr_in,char*);
int establish_initial_request_package(char *,ini_req_pack **,int);
int get(int,struct sockaddr_in ,char *);
int recv_data_pack(int ,struct sockaddr_in,char *,int,char *);
int file_open_and_write(char *,char *);
void send_err_pack(int ,struct sockaddr_in);
void send_ack_pack(int,struct sockaddr_in,int);
#endif
