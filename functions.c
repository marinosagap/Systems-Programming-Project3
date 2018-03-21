#include "header.h"
struct diaspora_array diaspora_buff[DIASPORA_SIZE];
Content_server * content_server_list;
int manager;//metablhth bool pou mas leei an grafei kapoios manager
int worker; //o arithmos twn worker pou diavazoun ton buffer
struct directories_buff *shared_buffer;
 int managers_active;
char *directory ;
int numDevicesDone;
int bytesTransferred,filesTransferred;
int error_connections ;//metablhth pou mas deixnei se posous Content server den kataferame na kanoume connect


void add_node(char * address, int port,char * dirorfile1,int delay1) //synarthsh pou dhmiourgei neo kobmo sthn lista gia enan Content Server
{
	Content_server * temp = malloc(sizeof(Content_server));
	if(temp == NULL)perror_exit("malloc");

	temp -> address= malloc((strlen(address) +1)* sizeof(char));
	if( temp ->address == NULL)perror_exit("malloc");
	strcpy(temp->address,address);
	temp->dirorfile1 = malloc((strlen(dirorfile1)+1) * sizeof(char));
	if(temp->dirorfile1 == NULL)perror_exit("malloc");
	strcpy(temp->dirorfile1,dirorfile1);
	temp ->delay1 = delay1;
	temp->port = port;
	//temp->socket = socket;
	temp ->next =content_server_list;
	content_server_list = temp;
	printf("Node added\n");
///	printf("ContentServerAddress1: %s  , ContentServerPort1: %d  , dirorfile1: %s  , delay1 :%d \n",temp->address,temp->port,temp->dirorfile1,temp->delay1);


}
int number_of_content_servers()//synarthsh pou mas epistrefei posous content server exoume 
{
	int count = 0;
	Content_server * temp = content_server_list;
	while(temp != NULL)
	{
		count++;
		temp = temp->next;
	}
	return count;
}
void destroy_content_server_list()
{
	Content_server * temp = content_server_list;
	while(temp != NULL)
	{
		Content_server * temp2 = temp ;
		temp = temp->next;
		free(temp2->address);
		free(temp2->dirorfile1);
		close(temp2->socket);
		free(temp2);

	}
}
void print_shared_buff()//synarthsh pou ektypwnei ton shared buff
{
	printf("Printing shared buffer \n\n");
	int i ;
	for(i = 0 ;i<SHARED_BUFF_SIZE;i++)
	{
		if( shared_buffer->data[i].ContentServerPort ==0)continue;
		printf("%s  , %s  , %d\n",shared_buffer->data[i].dirorfilename,shared_buffer->data[i].ContentServerAddress, shared_buffer->data[i].ContentServerPort);
		
	}
}
void print_content_server_list()//synarthsh pou apla ektypwnei sthn Content_server_list
{
	printf("Printing content server list ...");fflush(stdout);
	Content_server * temp = content_server_list;
	printf("\n\n");
	while(temp != NULL)
	{
		printf("ContentServerAddress1: %s  , ContentServerPort1: %d  , dirorfile1: %s  , delay1 :%d ,socket : %d\n",temp->address,temp->port,temp->dirorfile1,temp->delay1,temp->socket);
		fflush(stdout);
		temp = temp->next;
	}
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
int Worker_release()
{
	pthread_mutex_lock(&mtx);
		worker=0;
		struct directories_buff *pool = shared_buffer;

		//if(worker==0)
		//an den diavazei kaneis(dhladh den kanei kaneis consume) tote kanoume signal enan Manager
		{					//kai broadcast signal tous ypoloipous workers
			pthread_cond_signal(&write_cond);
			pthread_cond_broadcast(&read_cond);
		}
		if(managers_active <=0 && pool->count<=0)//&& active_managers==0 ) //an teleiwsan oloi oi Managers/producers kai den exoun meinei antikeimena ston buffer 
		{				
			numDevicesDone--;//aytos o worker kanei exit	//tote teleiwnoun kai oi worker
			if(numDevicesDone ==0 )//an exoun teleiwsei oloi oi worker
			{
				pthread_cond_signal(&allDone); //ksypname ton MirrorServer gia na steilei ta statistika ston MirrorInitiator
			}
			pthread_mutex_unlock(&mtx);
			return 0;//wra na teleiwsei h diadikasia twn worker
		}
	pthread_mutex_unlock(&mtx);
	return 1;

}/*
  char hello[] = "Hello World, Let me live."; // make this a char array not a pointer to literal.
        char *rest; // to point to the rest of the string after token extraction.
        char *token; // to point to the actual token returned.
        char *ptr = hello; // make q point to start of hello.

        // loop till strtok_r returns NULL.
        while(token = strtok_r(ptr, " ,", &rest)) {

                printf("%s\n", token); // print the token returned.
                ptr = rest; // rest contains the left over part..assign it to ptr...and start tokenizing again.    
        }*/
void create_folder(char * file)//opou file enai ths morfhs Worker/folder1/folder2/a.c
{
	printf("\t\t\tcreating folder file : %s\n\n",file);
	char sep ='/';

	char *rest; // to point to the rest of the string after token extraction.
    char *token; // to point to the actual token returned.
    char *ptr = file; // make q point to start of hello.
    char *directories;
    if( (directories =malloc(MSGSIZE*sizeof(char)) ) ==NULL)perror_exit(" directories malloc erorr");
    memset(directories,'\0',MSGSIZE);
    //strcpy(directories,directory);

    int count = 0 ;
     int strlen_token;
     char token_array[MSGSIZE][MSGSIZE];
     char command[MSGSIZE];
     int i =0;
     while(token = strtok_r(ptr,"/", &rest))
     {
     		//printf("token = %s\n",token);
            strlen_token = (int)strlen(token);
            count += strlen_token+1;
            strcpy(token_array[i],token);
            strcat(token_array[i++],"/");
            fflush(stdout);
           
            ptr = rest; // rest contains the left over part..assign it to ptr...and start tokenizing again.    
     }
     if(i ==0 )
     {
     	sprintf(directories," ");
     }

        int j ;
        for(j =0 ;j <i;j++)
        {
            //printf("%s\n",token_array[j]);
            if(j != i-1) strcat(directories,token_array[j]);
        }
            printf("directories : %s \n\n",directories);


    sprintf(command,"mkdir -p ");
    strcat(command,directories);
    printf("command = %s \n\n",command);
    FILE * pipe_fp;
    if ((pipe_fp = popen(command, "r")) == NULL )
                    perror("popen erorr");
	    	      // perror_exit("popen command ");
	if (errno == EINVAL || errno == EMFILE) perror("popen error");//perror_exit("popen command ");
    fclose(pipe_fp);
    //strcat(directories,token_array[j]);
    free(directories);

}
int  Worker_obtain()
{
	int index_array = -1;
	struct buffer_entries fetch_file;
	pthread_mutex_lock(&mtx);
		struct directories_buff *pool = shared_buffer;

		//while(num_of_items >0)//an exoun meinei akoma antikeimena gia na bgoun 
		{
			while( (worker || manager || (pool->count <= 0 && managers_active>0) ))// && managers_active >0) //an grafei kapoios h an 
			{//h an einai EMPTY o buffer tote ginetai wait  me thn proypothesh oti exoun meinei akoma antikeimena pou den exoun ginei consume
				if(manager)printf("Worker::>>a Manager is writting to buffer\n");
				else if (pool->count <= 0) printf("Worker::>>Buffer empty\n");

				pthread_cond_wait(&read_cond, &mtx);
			} 
			worker=1;
			

			//strcpy(file,pool->data[pool->start].dirorfilename);
			if(pool->count > 0) //an yparxoun antikeimena pou prepei na ginoun fetch
			{
				index_array = filesTransferred; //apothikebw thn timh ths metablhths filesTransfered 
				filesTransferred ++ ;//ayksanw ton arithmo twn arxeiwn pou metaferontai 
				memcpy(&fetch_file,&pool->data[pool->start],sizeof(struct buffer_entries) );		
			}
			else
			{
				memset(&fetch_file,'\0',sizeof(struct buffer_entries) );
			    fetch_file.ContentServerPort =0;
			}
			memset(&(pool->data[pool->start]),'\0',sizeof(struct buffer_entries) );

			pool->start = (pool->start + 1) % POOL_SIZE;
			pool->count--; 
			//printf("Worker::%ld >> just took : %d | ",(long int)pthread_self(),data);
			//print_buffer(pool);
			//if(num_of_items >0)
			//num_of_items--;//bgainei ena antikeimeno
		}
	pthread_mutex_unlock(&mtx);

	int return_value =Worker_release();//kanoume kai to release

	//edw ginetai to FETCH
	if(fetch_file.ContentServerPort!=0 )//elegxos gia to an exoun teleiwsei o manager
	{
		printf("Worker connecting to Content_servers : address  %s , port %d\n\n",fetch_file.ContentServerAddress,fetch_file.ContentServerPort);
		fflush(stdout);
		int socket = create_and_connect(fetch_file.ContentServerAddress,fetch_file.ContentServerPort);//dhmiourgia socket kai connection me ton ContentServer
		if(socket == 0 )//an den petyxei h syndesh 
		{
			printf("Did not manage to connect   to FETCH file : %s\n\n",fetch_file.dirorfilename);
		//	close(new_filedesc);
			close(socket);
			return return_value;

			
			pthread_exit(0);

		}
		printf("Worker Connnected successfully socket = %d \n\n",socket);
		char buf[MSGSIZE];
		memset(buf,'\0',MSGSIZE);

		sprintf(buf,"FETCH %d %s ",fetch_file.ContentServerID,fetch_file.dirorfilename);//opou 15 to id
		printf("WOKERKER :: FETCHING buf : %s \n\n",buf);
		//sprintf(buf,"FETCH 15 %s ",fetch_file.dirorfilename_as_asked);//zhtame to file 15 to id kai to stelnoume opws einai 
		while(write(socket ,buf,MSGSIZE)<0){} //stelnoume ston ContentServer LIST
		memset(buf,'\0',MSGSIZE);
		
		//strcpy(buf,directory);

		//strcat(buf,fetch_file.dirorfilename_as_asked);
		/*sprintf(buf,"%s%s",directory,fetch_file.dirorfilename_as_asked);//to buf exei to Worker/dirorfilename_as_asked

		char * directories = create_folder(buf);//opou file enai ths morfhs folder1/folder2/a.c*/
	//to directories einai to string me tous fakelous px folder1/folder2/folder3 //pou dhmiourghthike

		char c;
		
		char file_to_creat[MSGSIZE];
		//sprintf(file_to_creat,"%s%s",directory,fetch_file.dirorfilename_as_asked);
		

		/*strcpy(file_to_creat,directories);//to directories periexei olo to path mazi meto arxeio
		free(directories);
		strcat(file_to_creat,fetch_file.dirorfilename_as_asked);
	*/	 char ptr[MSGSIZE];
		strcpy(ptr,fetch_file.dirorfilename);//sto hole path
		 char * ret;
		 ret = strstr(fetch_file.dirorfilename, fetch_file.dirorfilename_as_asked);
 		sprintf(buf,"%s%s",directory,ret);//to buf exei to Worker/dirorfilename_as_asked

		 ret += strlen(fetch_file.dirorfilename_as_asked);

		 if(*ret =='/') //edw blepw an prokeitai gia directory
		 {
		 	printf("Worker:: prokeitai gia directory \n\n");
		 	fetch_file.dirorfilename_as_asked[strlen(fetch_file.dirorfilename_as_asked)]='/'; //epeidh einai array tou prosthegw "/"
			ret++;
		 }
		 //strtok_r(ptr,"folder1/", &rest);
		sprintf(file_to_creat,"%s%s%s",directory,fetch_file.dirorfilename_as_asked,ret);
		printf("ret = %s ,file_to_creat : %s  fetch_file.dirorfilename_as_asked : %s  fetch_file.dirorfilename: %s\n\n",ret,file_to_creat,fetch_file.dirorfilename_as_asked,fetch_file.dirorfilename);fflush(stdout);
		


		create_folder(buf);//opou file enai ths morfhs folder1/folder2/a.c*/

		int new_filedesc = creat(file_to_creat,0777);
		if(new_filedesc<0)perror_exit("newfile open error");
		printf("FETCHingggggn file : %s\n\n",fetch_file.dirorfilename);

		int counter=0;
		/*while((read(socket,&c,1) )<0 ) ;
		while((write(new_filedesc,&c,1) )<0);
*/

		while( (read(socket,&c,1)) >0)//diavazoume to arxeio byte byte
		{
			counter++;
			while((write(new_filedesc,&c,1) )<0); //stelnoume to arxeio byte byte
			while( (write(socket,&c,1)) <0);//tou stelnw pisw ena byte
		}
		diaspora_buff[index_array].bytes += counter;//apothikeboume ta bytes pou metaferthikan 
		/*pthread_mutex_lock(&transferred); //edw ayksanoume tous metrhtes byte kai arxeiwn metaforas
			filesTransferred++;
			bytesTransferred+= counter;
			printf("filesTransferred : %d , bytesTransferred: %d\n\n",filesTransferred,bytesTransferred);
		pthread_mutex_unlock(&transferred);
		*/
		
		close(new_filedesc);

		/*memset(buf,'\0',MSGSIZE);
		perror("############################################################################################\n\n\n");
		sprintf(buf,"YOUCANEND");
		printf("\n\nWorker:: sendind YOUCANENDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDd \n\n");fflush(stdout);

		while((write(socket,buf,MSGSIZE)) <0 );//stelnoume mhnyma ston ContentServer na teleiwsei 
		memset(buf,'\0',MSGSIZE);

		printf("\n\nWorker:: wainting to endddddddddddddddddddddddddddddddddddd \n\n");fflush(stdout);

		while((read(socket,buf,MSGSIZE)) <0);//perimenei na lavei kai mhnyma prin kleisei to socket
		*/
		close(socket);
		
		printf("Telos FETCHingggggn\n");
	}

	
	return return_value;

	//return Worker_release();

}

void * Worker(void * arg)
{
	
	printf("Worker..\n\n");
	while(Worker_obtain() )
	{
		//Worker_obtain();
	}
	printf("Worker : %ld Done managers_active : %d\n\n",pthread_self(),managers_active);

	pthread_exit(0);

}

void initialize_shared_buff(struct directories_buff *pool) {//initialization of shared buf
	pool->start = 0; // h arxh tou array
	pool->end = -1;		//to telos toyarray
	pool->count = 0;	//to count einai to posa antikeimena exei mesa
}

void Mirror_Manager_place(struct buffer_entries *data)//kanei insert sto shared buff to data
{
	pthread_mutex_lock(&mtx);
	
		struct directories_buff *pool = shared_buffer;

		while(worker  || manager || pool->count >= POOL_SIZE) // an diavazei kapoios worker h an grafei ston buffer allos manager 
		{//h an o buffer einai gematos
			printf("o Manager mpeike sto waiting \n\n");
			if(worker )printf("Manager::>>kapoios allos  worker diavazei/consume\n");
			if(manager)printf("Manager::>> another manager writes to buffer\n" );
			if(pool->count >= POOL_SIZE)printf("Manager::>>Buffer foul\n");

			pthread_cond_wait(&write_cond, &mtx);

		}
		manager = 1;//writer = true
		pool->end = (pool->end + 1) % POOL_SIZE;
		//data = num_of_items;
		//data = pthread_self();
		//pool->data[pool->end] = data;
		memcpy(&(pool->data[pool->end]),data,sizeof(struct buffer_entries));
		printf("Manager : %ld just placed to shaded buffer, file(holepath): %s  file(as asked) :%s  ,address: %s , port :%d\n\n",pthread_self(),data->dirorfilename,data->dirorfilename_as_asked,data->ContentServerAddress,data->ContentServerPort);
		pool->count++;
		//printf("Manager:: %ld >> just wrote : %d |  ",(long int)pthread_self(),data);
		//print_buffer(pool);
		pool = NULL;
	pthread_mutex_unlock(&mtx);
	//to relaease to kanw meta
	/*pthread_mutex_lock(&mtx);
		manager = 0;//manager = false
		pthread_cond_broadcast(&read_cond);//3ypname olous tous workers-consumers-readers 
		pthread_cond_signal(&write_cond); //3ypname enan allo manager-writer
	pthread_mutex_unlock(&mtx);

*/
}
//void Mirror_Manager_place(struct buffer_entries *data)//kanei insert sto shared buff to data
void Mirror_Manager_release()
{
	pthread_mutex_lock(&mtx);
			manager = 0;//manager = false
			pthread_cond_broadcast(&read_cond);//3ypname olous tous workers-consumers-readers 
			pthread_cond_signal(&write_cond); //3ypname enan allo manager-writer
	pthread_mutex_unlock(&mtx);

}
void Mirror_Manager_release_and_decrease_active_managers()
{
	pthread_mutex_lock(&mtx);
		manager = 0 ;
		managers_active--; //meiwnoume tous active managers 
		pthread_cond_broadcast(&read_cond);//3ypname olous tous workers
		pthread_cond_signal(&write_cond); //3ypname enan allo manager
	pthread_mutex_unlock(&mtx);

}
int contains_file(char  * file,char * buf)//synarthsh pou elegxei an yparxei to string-file mesa ston buff
{//an gyrisei 1 to file anhkei mesa sto buf alliws epistrefei 0
	char temp[MSGSIZE];
	char * temp_file = temp;
	strcpy(temp,file);

	if(temp_file[0] == '/')(temp_file)++;
	if(temp_file[strlen(temp_file)-1] =='/')
	{
		temp_file[strlen(temp_file)-1] ='\0';
	}
	char temp_buf[MSGSIZE];
	strcpy(temp_buf,buf);
 	char * token , *rest , *ptr = temp_buf;
    int i =0;
    //to buf einai ths morfhs /Desktop/folder1/folder113/a.c
    //an to file einai ena apo ta tokens pou ta xwrize me to "/" tote epistrefw 1
    while(token = strtok_r(ptr,"/", &rest))
    {
    	printf("token = %s temp_file = %s \n ",token,temp_file);
    	//if(!strcmp(file,"askisi1.zip"))sleep(3);

    	fflush(stdout);
   		if(!strcmp(token , temp_file))return 1 ;//anhkei 
    	ptr = rest; // rest contains the left over part..assign it to ptr...and start tokenizing again.    
    }
    return 0;

}
void * Mirror_Manager(void *arg) //tou pairname ws orisma ena struct Content_server kai kanoume thn syndesh 
{
	Content_server * temp = arg;
	int socket = create_and_connect(temp->address,temp->port);//dhmiourgia socket kai connection me ton ContentServer
	if(socket == 0 )//an den petyxei h syndesh 
	{
		printf("Did not manage to connect\n\n");
		Mirror_Manager_release_and_decrease_active_managers(); //eletherwnoume kai tous worker
		pthread_exit(0);

	}
	printf("Manager Connnected successfully socket = %d \n\n\n",socket);
	fflush(stdout);
	//sleep(5);
	char buf[MSGSIZE];
	sprintf(buf,"LIST %d %d ",temp->ID,temp->delay1); //ta evala twra statika to ID 
	printf("MANAGER ::: LIST MESSAGE : %s\n\n",buf);
	temp->socket = socket;
	while(write(temp->socket ,buf,MSGSIZE)<0){} //stelnoume ston ContentServer LIST
	FILE *sock_fp;
	if ((sock_fp = fdopen(temp->socket,"r+")) == NULL) 
				perror_exit("fdopen");

	size_t len = 0;
	memset(buf,'\0',MSGSIZE);
	int nread;
	struct buffer_entries *entry ;
	if((entry = malloc(sizeof(struct buffer_entries))) ==NULL)perror_exit("malloc error");
	while(1)//diavazei mexri na teleiwsei to LIST
	{
		//printf("waiting to read\n\n");fflush(stdout);
		while((read(temp->socket,buf,MSGSIZE)) < 0);

		if(!strcmp(buf,"END"))break;
		//printf("%s\n",buf);
		fflush(stdout);
		if(strstr(buf, temp->dirorfile1) != NULL)//an yparxei mesa sto string pou gyrise apo to LIST to arxeio pou theloume tote to topothetoume sto buffer
		//if(contains_file(temp->dirorfile1,buf)) //an to temp->dirrorfile1 yparxei sto buf tote to grafoume sto arrahy
		{

			printf("Manager :: einai mesa sto List to %s , buf string : %s\n\n",temp->dirorfile1,buf);fflush(stdout);
			//sleep(3);
			strcpy(entry->dirorfilename,buf);
			strcpy(entry->dirorfilename_as_asked, temp->dirorfile1);
			//strcpy(entry->dirorfilename,temp->dirorfile1);
			strcpy(entry->ContentServerAddress,temp->address);
			entry->ContentServerPort = temp->port;
			entry->ContentServerID = temp->ID;


			Mirror_Manager_place(entry);
			//sleep(2);
			Mirror_Manager_release();

			//Mirror_Manager_release_and_decrease_active_managers();//kanoume kai active_managers -- kai ksypname enan allo manager kai olous tous workers

		}
		//kataskevazw to entry pou tha grapsei o Mirror Manager sto shared buffer
		
		usleep(500);//gia kalytero sygxronismo twn manager
		memset(buf,'\0',MSGSIZE);
		//sleep(1);
	}
	Mirror_Manager_release_and_decrease_active_managers();//kanoume kai active_managers -- kai ksypname enan allo manager kai olous tous workers


	free(entry);
	/*memset(buf,'\0',MSGSIZE);
	sprintf(buf,"YOUCANEND");
	while((write(socket,buf,MSGSIZE)) <0 );//stelnoume mhnyma ston ContentServer na teleiwsei 
*/
	close(socket);
	fclose(sock_fp);
	printf("\n\nTELEIWSE TO LIST\n\n");
	
	pthread_exit(0);

	//an pe

}
void * initiator_function(int initiator_sock)//arxikh epikoinwnia me ton MirrorInitiator
{

	char buf[MSGSIZE];
	memset(buf,'\0',sizeof(buf));

//	while(read(initiator_sock,buf,MSGSIZE) < 0){} //diavazoume apo ton mirror initiator to megethos tou string me tis diefthynseis twn ContentServer
 
 //	int string_size = atoi(buf);
 	char ContentServer[MSGSIZE]; //to string_size einai dynamiko kathe fora
 	memset(ContentServer,'\0',MSGSIZE);

 	while(read(initiator_sock,ContentServer,MSGSIZE) <0){} //diavazoume apo ton mirror initiator to string me tis diefthynseis twn ContentServer
 	

	printf("Received from initiator : %s\n\n",ContentServer);
Content_servers2(ContentServer);
	fflush(stdout);
	
}
//ContentServerAddress1:ContentServerPort1:dirorfile1:delay1
int * Content_servers(char * ContentServer)//sunarthsh pou dhmiourgei thn syndesh me tous Content server kai epistrefei pointer sta 
{
	char address[MSGSIZE],port[MSGSIZE],dirorfile1[MSGSIZE],delay1[MSGSIZE],buf[MSGSIZE];
	
	memset(buf,'\0',MSGSIZE);
	int i= 0;
	int n ;
	char * str=ContentServer;
	
	int count = 0 ;

	while (sscanf(str, "%[^:]%n", buf, &n))
	{
            printf("%s\n", buf);
            if(count == 0)strcpy(address,buf);
            else if (count==1)strcpy(port,buf);
            else if(count ==2)
            {
            	strcpy(dirorfile1,buf);
            	str += n+1;
            	memset(buf,'\0',MSGSIZE);
            	sscanf(str, "%[^,]%n", buf, &n);
            	strcpy(delay1,buf);
           		printf("%s\n", buf);
                        	//printf("\t\t\tContenttttttttttttttttttttttt\n\n");
				//int socket = create_and_connect(address ,atoi(port));
				add_node( address,atoi(port),dirorfile1,atoi(delay1));  

            	count =-1;
            }

            count++;
            str += n+1;

            if (*str == '\0')
                    break;
            if(*(str) ==',')
            {
            	count=0;
            	printf("\t\t\tContenttttttttttttttttttttttt\n\n");
            }
    }
}
int * Content_servers2(char * ContentServer)//sunarthsh pou dhmiourgei thn syndesh me tous Content server kai epistrefei pointer sta 
{
	char address[MSGSIZE],port[MSGSIZE],dirorfile1[MSGSIZE],delay1[MSGSIZE],buf[MSGSIZE];
	memset(buf,'\0',MSGSIZE);
	memset(address,'\0',MSGSIZE);
	memset(port,'\0',MSGSIZE);
	memset(dirorfile1,'\0',MSGSIZE);
    memset(delay1,'\0',MSGSIZE);

	strcpy(buf,ContentServer);

	char *rest; // to point to the rest of the string after token extraction.
    char *token; // to point to the actual token returned.
    char *ptr = buf; // make q point to start of hello.
    char *directories;
	printf("buf = %s \n\n",buf);
 	while(token = strtok_r(ptr,",", &rest))//to spame arxika me thn koma 
    {
     	printf("token = %s\n",token);
       	
       	char * inside_token;
		
		char *inside_rest; // to point to the rest of the string after token extraction.
    	char *inside_ptr = token; // make q point to start of hello.
    	int count  =0;
       	while(inside_token= strtok_r(inside_ptr,":",&inside_rest))
       	{
       		printf("inside_token = %s\n",inside_token);fflush(stdout);
       		if(count ==0)
       		{
  			    memset(address,'\0',MSGSIZE);
       			strcpy(address,inside_token);
       		}
       		else if(count ==1)
       		{
       			memset(port,'\0',MSGSIZE);
      			strcpy(port,inside_token);
       		}
       		else if(count ==2)
       		{
   			    memset(dirorfile1,'\0',MSGSIZE);

       			strcpy(dirorfile1,inside_token);
       		}
       		else if(count ==3)
       		{
       			memset(delay1,'\0',MSGSIZE);
       			strcpy(delay1,inside_token);

       		}
       		count++;
       		inside_ptr = inside_rest;

       	}          
       	add_node( address,atoi(port),dirorfile1,atoi(delay1));
 
        ptr = rest; // rest contains the left over part..assign it to ptr...and start tokenizing again.    
    }

}

int create_and_connect(char *address ,int port)//synarthsh pou pairnei enan ta xarakthristika enos content server , dhmiourgei ena socket kai kanei connect se ayton 
{//epistrefei 0 an den syndethei kai sock an syndethei
	int sock;
	char servername[MSGSIZE];
	memset(servername,'\0',MSGSIZE);
	strcpy(servername,address);
	
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct hostent *serverhost;

	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){perror("error in socket creation\n");exit(1);}

	serverhost = gethostbyname(servername);
	if(serverhost == NULL)
		//perror_exit("gethostbyname:");
	{
		perror("didnot manage to connect");
		return 0;
	}
	server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, serverhost->h_addr, serverhost->h_length);
    server.sin_port = htons(port);         /* Server port */

    printf("Trying to connect");
 	int i ;
 	for(i=0;i<10;i++) //exw valei 5 fores na prospathisei na ginei to Connnect ston Content server 
 		{
 			if((connect(sock, serverptr, sizeof(server)) )>= 0)break;
 			sleep(1);//kathe fora pou prospathei na syndethei kai apotygxanei tha periemenei alla 2 sec
 		}
 	if(i<10)printf("\nConnected to %s successfully\n\n",address);
 	else 
 	{
		error_connections++;

 		printf("\nDidnot connect to %s \n\n",address);
		return 0;
	}   //perror_exit("connect");	
	return sock;//epistrefoume to socket
}
