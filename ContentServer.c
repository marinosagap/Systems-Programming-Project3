#include "Content_header.h"


int readers=0;//metablhth pou mas deixnei to plhthos twn reader
int writer=0;	//metablhth boolean pou deixnei an kapoios writer einai sto cs
void writer_fun(int id, int delay)
{
	pthread_mutex_lock(&delay_mtx);
		while(readers>0 || writer)//an diavazei kapoios h an grafei kapoios tote ginetai o writer suspended
			pthread_cond_wait(&write_cond, &delay_mtx);
		writer = 1;
			
	pthread_mutex_unlock(&delay_mtx);

	ContentServerID_list[array_index].ContentServerID = id;
	ContentServerID_list[array_index].delay = delay;
	array_index++;

	pthread_mutex_lock(&delay_mtx);
		writer = 0;
		pthread_cond_broadcast(&read_cond);//3ypname olous tous readers 
		pthread_cond_signal(&write_cond); //3ypname enan allo writer
		
	pthread_mutex_unlock(&delay_mtx);

}

int reader_fun(int id)//psaxnoume to delay pou edwse to LIST gia na paroume to delay gia to fetch
{
	pthread_mutex_lock(&delay_mtx);
		while(writer)//an kapoios writer grafei 
			pthread_cond_wait(&read_cond, &delay_mtx);
		readers++;	
	pthread_mutex_unlock(&delay_mtx);

	int i =0;
	int return_value=0;
	//printf(" trying to find id == %d\n\n\n",id);fflush(stdout);
	for(i = 0 ;i< array_index;i++)
	{
		if(ContentServerID_list[i].ContentServerID == id  )
		{
			return_value = ContentServerID_list[i].delay; //brethike to id pou dwthike apo to LIST
			//memset(&ContentServerID_list[i],'\0',sizeof(struct_for_delay));//
			//ContentServerID_list[i].ContentServerID =-1;
			//ContentServerID_list[i].delay=0; //to afairoume sthn ousia apo ton pinaka
			break;
		}
	}
	pthread_mutex_lock(&delay_mtx);
		readers--;
		if(readers==0)
			pthread_cond_signal(&write_cond); //3ypname enan allo writer
		
	pthread_mutex_unlock(&delay_mtx);
	return return_value;
}
void * Content_server_thread(void *arg)
{
	Content_thread thread ;
	memcpy(&thread,arg,sizeof(Content_thread));

	char buf[MSGSIZE];
	FILE * sock_fp, *pipe_fp;
	///LISTTT
	int newsock = thread.newsock;

	while(read(newsock,buf, MSGSIZE)<0){}//perimenei na diavasei 

	if(!memcmp(buf,"LIST",strlen("LIST")))
	{
		printf("%s\n", buf);
		char temp1[MSGSIZE],temp2[MSGSIZE],temp3[MSGSIZE];
		sscanf(buf,"%s %s %s",temp1,temp2,temp3);
		//o buf = "LIST <ContentServerID> <delay>
//writer
		

		/*thread.ContentServerID= atoi(temp2); 
		thread.delay = atoi(temp3);
*/
		    char command[MSGSIZE];//to command prepei na einai ls -allR | find

		 	sprintf(command,"find %s -type f",directory);
		if ((sock_fp = fdopen(newsock,"w+")) == NULL)
		{
			fclose(sock_fp);
			close(newsock);
			pthread_detach(thread.thread_id);

			perror_exit("fdopen");

		} 

	 	if ((pipe_fp = popen(command, "r")) == NULL )
	    	       perror_exit("popen command ");
	    if (errno == EINVAL || errno == EMFILE) perror_exit("popen command ");
	    //if(!strcmp(errno,EINVAL) || !strcmp(errno,EMFILE))perror_exit("popen command ");
	    /* transfer data from ls to socket */
        size_t len = 0;
  		memset(buf,'\0',MSGSIZE);
  		int nread;
  		int listing_counter = 0;
	    while( fgets(buf,MSGSIZE,pipe_fp)!=NULL )
	    {
	    	buf[strlen(buf)-1]='\0';
	    	while( write(newsock,buf,MSGSIZE)<0){}//stelnoume ena ena ta apotelesmata tou find se grammes
 	    	memset(buf,'\0',MSGSIZE);
 	    	listing_counter++;
		}
		//printf("End of LIST listing_counter ======%d \n\n",listing_counter);
		fflush(stdout);
		memset(buf,'\0',MSGSIZE);
		sprintf(buf,"END");
		while( write(newsock,buf,MSGSIZE)<0){}//stelnoume sto string END ws endeiksh oti teleiwse to LIST
		fclose(sock_fp);
		fclose(pipe_fp);

		writer_fun(atoi(temp2),atoi(temp3));
	}
	///FETCH

	if (!memcmp(buf,"FETCH",strlen("FETCH")))
	{
		printf("%s\n\n",buf);
		//printf("FETCH : %s \n",buf);
		//sleep(thread.delay);//kanoume toso sleep
		char temp1[MSGSIZE],temp2[MSGSIZE],file[MSGSIZE];
		sscanf(buf,"%s%s%s",temp1,temp2,file);
		//FETCH <ContentServerID> <dirrofile_name (foul path edw)>
		int i ;
		/*for(i=0;i<CONTENT_THREADS;i++)
		{

		}*/

		char file_to_fetch[MSGSIZE];
		sprintf(file_to_fetch, "%s",file);
		
		int fetch_delay = reader_fun(atoi(temp2));//perimenoume na paroume to delay
		printf("sleeping for fetch_delay :%d ID : %d\n\n",fetch_delay, atoi(temp2));fflush(stdout);


		if(fetch_delay > 0) sleep(fetch_delay);
		//sprintf(file_to_fetch,"%s%s",directory,file);
		//strcpy(file_to_fetch,directory);
		//strcat(file_to_fetch, file);//ayto doulebei swsta mono an directory einai ths morfhs folder/ h ./
		printf("FETCH : fetching file %s||\n\n",file_to_fetch);
		printf("ContentServer  trying to open : %s \n\n",file_to_fetch);
		int filedesc = open(file_to_fetch,O_RDONLY);

		if(filedesc<0)
		{
			close(filedesc);
			close(newsock);

			pthread_detach(thread.thread_id);
			perror_exit("error opening file ");
		}
		char c ;
		int count = 0 ;
		while(read(filedesc,&c,1)>0)//diavazoume to arxeio byte byte
		{	count++;
			char b;
			do 
			{
				
				while(write(newsock,&c,1)<0); //stelnoume to arxeio byte byte
				while(read(newsock,&b,1)<0);//perimenei na diavasei kiolas
			}while(b != c);//elegxos an epestrepse o MirrorManager elabe ton swsto xarakthra
		}
		memset(buf,'\0',MSGSIZE);

		/*printf("Waiting for message to close sock\n");fflush(stdout);
		while((read(newsock,buf,MSGSIZE)) <0);//perimenei na pari mhnyma gia na teleiwsei
		if(!strcmp(buf,"YOUCANEND"))
		{
			printf("Content Server thread endsssssssssssssssss\n\n");
			fflush(stdout);
		}
		printf("End of FETCH egrapse %d \n\n",count );
		fflush(stdout);
		memset(buf,'\0',MSGSIZE);
		while(write(newsock,buf,MSGSIZE)<0);*/
		

		close(filedesc);

	}
	//sleep(3);
	/*int i  = 0 ;
	for(i = 0;i<1000000000;i++);
	*/
	/*while((read(newsock,buf,MSGSIZE)) <0);//perimenei na pari mhnyma gia na teleiwsei
	if(!strcmp(buf,"YOUCANEND"))
	{
		printf("Content Server thread endsssssssssssssssss\n\n");
		fflush(stdout);
	}*/
	shutdown(newsock,1);
	//close(newsock);

	pthread_detach(thread.thread_id);

}

int main(int argc,char** argv)
{
	if(argc !=5 )
	{
		perror_exit("Dwthike lathos arithmos orismatwn");
	}

		//prod_cons();
	int i =0;
	for(i=1;i<argc;i++)
	{
		if(!strcmp(argv[i],"-p"))//port
			port = atoi(argv[i+1]);
		else if(!strcmp(argv[i],"-d"))//directory
			strcpy(directory, argv[i+1]);
	

	}
	struct stat st = {0};
	if (stat(directory, &st) == -1)perror_exit("Wrong : directory doesnot exist ");//se periptwsh pou dwthei lathos directory
	int  sock,err;
	int newsock[CONTENT_THREADS];
	struct sockaddr_in server, client;
	socklen_t clientlen;
	struct sockaddr * serverptr = (struct sockaddr *) &server,
	*clientptr = (struct sockaddr * ) &client;
	struct hostent *clienthost;
	pthread_t *tids;
	if (( tids = malloc ( CONTENT_THREADS * sizeof ( pthread_t ) ) ) == NULL ) { perror( " malloc " );exit(1) ; }

	//pthread_mutex_init(&mtx, 0);
//	pthread_cond_init(&cond_var, 0);

//	pthread_cond_destroy(&cond_var);


	//creating Socket 
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){perror("error in socket creation\n");exit(1);}
	if( ( bind_socket_to_address(server,sock,port)) <0){perror("Error sto bind\n");exit(1);}

	if(listen(sock,CONTENT_THREADS) <0){perror("listen Erorr\n");exit(1);}
	printf("listening.. to port %d \n",port);
	int initiator_sock;
	char buf[MSGSIZE];
	FILE * sock_fp, *pipe_fp;
	
	Content_thread * thread;
	if((thread = malloc(CONTENT_THREADS* sizeof(Content_thread))) ==NULL)perror_exit("malloc");

	//if((ContentServerID_list = malloc(CONTENT_THREADS *sizeof(struct_for_delay))))perror_exit("malloc");
	pthread_mutex_init(&delay_mtx,0);


	while(1)
	{
		memset(buf,'\0',MSGSIZE);
		newsock[i] = accept( sock,clientptr,&clientlen);//kanei accept enan client kai pairnoume ta stoixeia tou 
		if(newsock[i] < 0){perror("Error accepting \n");exit(1);}	
		//printf("Connection with a Manager established\n\n");
		fflush(stdout);
		thread[i].newsock = newsock[i];
		thread[i].thread_id = tids[i];
		//stcpy(thread[i].directory,directory);
		pthread_create(tids+i, NULL, Content_server_thread,(void*)(thread+i) );//dhmiourgw thread gia osous 
		usleep(500);
		i++;
	}
	close(sock);
	free(tids);
	pthread_mutex_destroy(&delay_mtx);

	//free(ContentServerID_list);
	free(thread);

}

void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
int bind_socket_to_address(struct sockaddr_in server,int sock ,int port) //kanei bind sto socket
{
	memset(&server, 0, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port) ; 
	return bind(sock,(struct sockaddr *)&server,sizeof(server));
}