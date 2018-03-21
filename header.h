#include <stdio.h>
#include <sys/wait.h>
/* sockets */
#include <sys/types.h>
#include <sys/stat.h>

/* sockets */
#include <sys/socket.h>
/* sockets */
#include <netinet/in.h>
/* internet sockets */
#include <netdb.h>
/* g e t h o s t b y a d d r */
#include <unistd.h>
/* fork */
#include <stdlib.h>
/* exit */
#include <ctype.h>
/* toupper */
#include <signal.h>
#include <fcntl.h>
/* signal */
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#define MSGSIZE 400
#define SHARED_BUFF_SIZE 30
#define POOL_SIZE 30
#define DIASPORA_SIZE 10000//max megethos arxeiwn
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

struct diaspora_array
{
	long int bytes;
};

extern struct diaspora_array diaspora_buff[DIASPORA_SIZE];

struct buffer_entries 			//o buffer gia ths diamoirazomenhs metablhths
{
	char dirorfilename[MSGSIZE],ContentServerAddress[MSGSIZE],dirorfilename_as_asked[MSGSIZE];
	int ContentServerPort,ContentServerID;
};
struct directories_buff{
	struct buffer_entries data[SHARED_BUFF_SIZE];
	int start;
	int end;
	int count;
} ;

extern char *directory;

extern struct directories_buff *shared_buffer;
void Mirror_Manager_place(struct buffer_entries *data);//kanei insert sto shared buff to data
void print_shared_buff();//synarthsh pou ektypwnei ton shared buff
void Mirror_Manager_release(); //kanei release ton mutex kai signal enan allo manager kai broadcast olous tous workers
void Mirror_Manager_release_and_decrease_active_managers();//kanei to idio me ton release apla kanei kai thn metavlhth active_managers--

int Worker_release();

void initialize_shared_buff(struct directories_buff *pool);//initialization of shared buf

pthread_cond_t allDone;
extern int error_connections ;//metablhth pou mas deixnei se posous Content server den kataferame na kanoume connect
pthread_cond_t read_cond; //condition variable gia diavasma apo to buffer
pthread_cond_t write_cond;//condition variable gia grapsimo sto buffer
extern int numDevicesDone ;
extern int manager;//metablhth bool pou mas leei an grafei kapoios manager
extern int worker; //o arithmos twn worker pou diavazoun ton buffer
pthread_mutex_t mtx,transferred;
extern int bytesTransferred ,filesTransferred;

extern int managers_active;//metablhth pou xrhsimopoiw sto elegxo telous twn Worker
typedef struct Content_server
{
	int ID; //kathe syndesh manager Content Server tha exei ena id (etsi kai alliws kathe manager ena LIST kanei opote xrhsimopoiw to unique id ayto ws ContentServerID)
	int socket;
	char *address;
	int port ;
	char * dirorfile1;
	int delay1;
	struct Content_server * next;
}Content_server;

extern Content_server * content_server_list;
//int port;
void perror_exit(char *message);
void * Mirror_Manager(void *arg); //synarthsh tou mirormanager thread ,,tou pairname ws orisma ena struct Content_server
int contains_file(char  * file,char * buf);//synarthsh pou elegxei an yparxei to string mesa ston buff

void * initiator_function();
void create_folder(char * file);//opou file enai ths morfhs folder1/folder2/a.c


int create_and_connect(char *address ,int port);//synarthsh pou pairnei enan ta xarakthristika enos content server , dhmiourgei ena socket kai kanei connect se ayton 

//arxika h lista me tous Content_servers einai kenh 
void	print_content_server_list();

void add_node(char * address, int port,char * dirorfile1,int delay1); //synarthsh pou dhmiourgei neo kobmo sthn lista gia enan Content Server

int number_of_content_servers();///synarthsh pou epistrefei ton arithmo ton Content server / Manageer

void destroy_content_server_list();//apodesmeysh ths listas


void print_content_server_list();//synarthsh pou apla ektypwnei sthn Content_server_list

int bind_socket_to_address(struct sockaddr_in server,int sock ,int port); //kanei bind sto socket


void * Mirror_Manager(void *arg); //h function tou Mirror Manager thread ,,tou pairname ws orisma ena struct Content_server
void * Worker(void * arg);//h function tou Worker thread

void * initiator_function(int initiator_sock);//h


int * Content_servers(char * ContentServer);//sunarthsh pou dhmiourgei thn syndesh me tous Content server kai epistrefei pointer sta 
int * Content_servers2(char * ContentServer);//sunarthsh pou dhmiourgei thn syndesh me tous Content server kai epistrefei pointer sta 


//ContentServerAddress1:ContentServerPort1:dirorfile1:delay1
/*
serverhost = gethostbyname(servername);
	if(serverhost == NULL)
		perror_exit("gethostbyname:");

	 server.sin_family = AF_INET;      
    memcpy(&server.sin_addr, serverhost->h_addr, serverhost->h_length);
    server.sin_port = htons(port);   
*/
