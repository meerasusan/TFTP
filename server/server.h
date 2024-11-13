#ifndef SERVER
#define SERVER

#define PORT 5000
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define SERVER_LENGTH 5
#define WORD_COUNT 30
#define MAX_DATA 516
#define DATA_SIZE 512
#define TIMEOUT 30//30 sec
#define LIMIT 2
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERR 5



typedef struct RRQ_WRQ_packet
{
	unsigned short opcode;
	char fname[WORD_COUNT];
	char byte0;
	char mode[WORD_COUNT];
	char byte_end;
}ini_recv_pack;

typedef struct data_packet
{
	unsigned short opcode;
	unsigned short block_no;
	char data[DATA_SIZE];
}data_pack;

typedef struct ack_packet
{
	unsigned short opcode;
	unsigned short block_no;
}ack_pack;

typedef struct error_packet
{
	unsigned short opcode;
	unsigned short error_code;
//	char err_msg[WORD_COUNT];
//	char byte_0;
}err_pack;

void define_rrq(int,struct sockaddr_in *,int ,char *,char *);
void define_wrq(int,struct sockaddr_in *,int ,char *,char *);
void define_data(int,struct sockaddr_in *,int, char *,char *);
//void define_ack(int,struct sockaddr_in *,char *);
void define_err(int,struct sockaddr_in *,int,char *,char *);


#endif
