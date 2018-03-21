#include "header.h"
Content_server * content_server_list= NULL;
int port;
struct diaspora_array diaspora_buff[DIASPORA_SIZE];
struct directories_buff *shared_buffer=NULL;//o shared buffer twn Worker , Manager
int bytesTransferred=0 ,filesTransferred=0;
int manager=0;//metablhth bool pou mas leei an grafei kapoios manager
int worker=0; //o arithmos twn worker pou diavazoun ton buffer
int managers_active=1;
char *directory = NULL;
int numDevicesDone =0;//mas leei posoi worker einai energoi
int error_connections=0 ;//metablhth pou mas deixnei se posous Content server den kataferame na kanoume connect


int main(int argc,char** argv)
{
	if(argc !=7 )
	{
		perror_exit("Dwthike lathos arithmos orismatwn");
	}

		//prod_cons();
	int i =0;
	if((directory = malloc(MSGSIZE* sizeof(char))) ==NULL)perror_exit("malloc error");
	int thread_num;
	for(i=1;i<argc;i++)
	{
		if(!strcmp(argv[i],"-p"))//port
			port = atoi(argv[i+1]);
		else if(!strcmp(argv[i],"-m"))//directory
			strcpy(directory, argv[i+1]);
		else if(!strcmp(argv[i],"-w"))//threadnum
			thread_num = atoi(argv[i+1]);

	}
	numDevicesDone = thread_num; //to arxikopoiw me ton arithmo twn worker

	if(directory[strlen(directory)-1] !='/')strcat(directory,"/");//prosthetw to "/"
	/*int status;
	//strcat(directory," -p");
	status = mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if(status ==-1){perror_exit("error in mkdir");}*/
	struct stat st = {0};

	if (stat(directory, &st) == -1) {
    	mkdir(directory, 0777);
	}
	pthread_mutex_init(&mtx, 0);
	pthread_mutex_init(&transferred,0);

	if( (shared_buffer = malloc(sizeof(struct directories_buff)) )==NULL)perror_exit("malloc errror");
	initialize_shared_buff(shared_buffer);

	pthread_cond_init(&write_cond, 0);
	pthread_cond_init(&read_cond, 0);
	pthread_cond_init(&allDone, 0);


	int  sock,err;
	int newsock[10];
	struct sockaddr_in server, client;
	socklen_t clientlen;
	struct sockaddr * serverptr = (struct sockaddr *) &server,
	*clientptr = (struct sockaddr * ) &client;
	struct hostent *clienthost;
	
	//creating Socket 
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){perror("error in socket creation\n");exit(1);}
	if( ( bind_socket_to_address(server,sock,port)) <0){perror("Error sto bind\n");exit(1);}

	pthread_t *workers;//worker threads
	if (( workers = malloc ( thread_num * sizeof ( pthread_t ) ) ) == NULL ) { perror( " malloc " );exit(1) ; }
	for(i = 0;i<thread_num;i++)//dhmiourgia worker threads
	{
		pthread_create(workers+i, NULL, Worker,NULL);
	}
	


	if(listen(sock,100) <0){perror("listen Erorr\n");exit(1);}
	printf("listening.. to port %d \n",port);
	int initiator_sock;
	initiator_sock = accept( sock,clientptr,&clientlen);//kanei accept enan client kai pairnoume ta stoixeia tou 
	if(initiator_sock < 0){perror("Error accepting \n");exit(1);}	
	printf("Connection with initiator established\n\n");
	fflush(stdout);
	initiator_function(initiator_sock);

	print_content_server_list();

	int content_server_num = number_of_content_servers();
	printf("content_server_num = %d\n\n\n",content_server_num);
	pthread_t *managers;
	if (( managers = malloc ( content_server_num * sizeof ( pthread_t ) ) ) == NULL ) { perror( " malloc " );exit(1) ; }

	//ftiaxnoume osa thread einai kai oi manager-content servers
	Content_server * temp = content_server_list;
	managers_active = content_server_num;
	for(i = 0;i<content_server_num && (temp != NULL);i++)//dhmiourgoume Managers osa kai o arithmos twn Content Server
	{
	
		//DHMIOURGIA ENOS MANAGER THREAD GIA KATHE CONTENTSERVER
		temp->ID = i;
		pthread_create(managers+i, NULL, Mirror_Manager,(void * )temp);
		temp = temp->next;
	}
	
	
	
	for (i=0 ; i<content_server_num; i++)
       if (err = pthread_join(*(managers+i), NULL)) {
           perror2("pthread_join", err); exit(1); }
	for (i=0 ; i<thread_num; i++)
       if (err = pthread_join(*(workers+i), NULL)) {
           perror2("pthread_join", err); exit(1); }

		//for(i = 0;i<5;i++)close(newsock[i]);


	/*if (err = pthread_mutex_destroy(&mtx)) { /// Destroy mutex 
           perror2("pthread_mutex_destroy", err); exit(1); }
*/

       //    printf("telos MirrorServer\n\n");
	pthread_mutex_lock(&mtx);
		while(numDevicesDone > 0) //o mirror server perimenei se ayto to variable
		{
			pthread_cond_wait(&allDone, &mtx);
		}
	pthread_mutex_unlock(&mtx);

	printf("telos MirrorServer\n\n");fflush(stdout);
	sleep(3);
	//stelnoume twra ta statistika ston Initiator
	char buf[MSGSIZE];
	memset(buf,'\0',MSGSIZE);

	int j =0;
	while(j<filesTransferred)
	{
		bytesTransferred+= diaspora_buff[j].bytes;
		j++;
	}

	float x_meso = bytesTransferred/(float)filesTransferred;
	double diaspora = 0;
	for(j =0;j<filesTransferred;j++)
	{
		diaspora += ((float)diaspora_buff[i].bytes - x_meso)*((float)diaspora_buff[i].bytes - x_meso);
	} 
	diaspora = diaspora / (float)(filesTransferred-1);

	float mesos_oros = (float)bytesTransferred/(float)filesTransferred;
	
	sprintf(buf,"%d %d %f %lf %d",bytesTransferred ,filesTransferred,mesos_oros,diaspora,error_connections);
	while(write(initiator_sock,buf, MSGSIZE)<0 );//ta stelnoume ston INitiator

    pthread_cond_destroy(&write_cond);
	pthread_cond_destroy(&read_cond);
	pthread_cond_destroy(&allDone);
	pthread_mutex_destroy(&mtx);

print_shared_buff();//synarthsh pou ektypwnei ton shared buff
	destroy_content_server_list();

	close(initiator_sock);

	close(sock);
	free(workers);
	free(managers);
	free(directory);
	free(shared_buffer);

}
