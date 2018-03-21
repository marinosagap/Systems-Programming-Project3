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
#define SHARED_BUFF_SIZE 400
#define POOL_SIZE 400
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))



int port;
#define CONTENT_THREADS 400
typedef struct Content_thread
{
	int newsock;//to socket apo opou diavazei
	pthread_t thread_id; //to id tou thread 
}Content_thread;

typedef struct struct_for_delay//struct sto opoio apothikeboume ID ContentServer kai delay pou mas erxetai apo to LIST
{
	
	int ContentServerID;
	int delay;
}struct_for_delay;
struct_for_delay ContentServerID_list[CONTENT_THREADS];
int array_index = 0;
//void perror_exit(char *message);

char directory[MSGSIZE]; //global metablhth pou einai to directory pou o sygkekrimmenos Content Server eksyphretei
pthread_mutex_t delay_mtx;
void * Content_server_thread(void *arg);
int bind_socket_to_address(struct sockaddr_in server,int sock ,int port); //kanei bind sto socket
void perror_exit(char *);

pthread_cond_t read_cond; //condition variable gia diavasma apo to buffer
pthread_cond_t write_cond;//condition variable gia grapsimo sto buffer
extern int readers;//metablhth bool pou mas leei an grafei kapoios manager
extern int writer; //o arithmos twn worker pou diavazoun ton buffer
int reader_fun(int id);//psaxnoume to delay pou edwse to LIST gia na paroume to delay gia to fetch
void writer_fun(int id, int delay);
