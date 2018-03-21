#include "header.h"

int main(int argc,char** argv)
{
	if(argc <=6 )
	{
		perror_exit("Dwthike lathos arithmos orismatwn");
	}

		//prod_cons();
	int i =0,port;
	char directory[MSGSIZE], MirrorServerAddess[MSGSIZE],*ContentServers,servername[MSGSIZE];
	
	int thread_num;
	for(i=1;i<argc;i++)
	{
		if(!strcmp(argv[i],"-n"))					//MirrorServerAddess
			strcpy(MirrorServerAddess,argv[i+1]);
		else if(!strcmp(argv[i],"-p"))				//MirrorServerPORT
			port = atoi(argv[i+1]);
		else if(!strcmp(argv[i],"-s"))				//stoixeia gia tous ContentServers
		{
			ContentServers = malloc(strlen(argv[i+1])+10);
			strcpy(ContentServers,argv[i+1]);

		}			

	}
	int sock;
	//char buf[256];
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct hostent *serverhost;

	memset(servername,'\0',30);
	strcpy(servername,MirrorServerAddess);

	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){perror("error in socket creation\n");exit(1);}

	serverhost = gethostbyname(servername);
	if(serverhost == NULL)
		perror_exit("gethostbyname:");

	 server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, serverhost->h_addr, serverhost->h_length);
    server.sin_port = htons(port);         /* Server port */
    printf("Connecting to : %s port: %d \n",serverhost->h_name,port);

 	while ((connect(sock, serverptr, sizeof(server)) )< 0){}//kolla edw mexri na syndethei
	   //perror_exit("connect");	
 	

	printf("Just Connected to : %s port: %d \n",serverhost->h_name,port);
	struct in_addr ** addr_list = ( struct in_addr **) serverhost->h_addr_list ;

	for ( i = 0; addr_list[i] != NULL ; i++) {
		printf ( " %s resolved to %s \n " ,serverhost-> h_name ,(char*)inet_ntoa(*addr_list[i]) ) ;
	}

	char buf[MSGSIZE];
	/*sprintf(buf,"Mirror_Initiator");
	if ( write(sock,buf,100) < 0) perror_exit("write");
	///sleep(1);
	if(read(sock,buf,100) <0) perror_exit("read");
*/

	//sprintf(buf,"%d",(int)strlen(ContentServers));//vazw ston buffer to megethos tou string kai to stelnw
	//strcpy(buf,strlen(ContentServers) );
	
	//strcpy(buf,ContentServers ); ///kai twra stelnoume ston MirrorServer to ta stoixeia twn ContentServer
	ContentServers[strlen(ContentServers)-1]='\0';
	if ( write(sock,ContentServers,strlen(ContentServers)) < 0) perror_exit("write");


	//printf("Received from MirrorServer buf: %s \n\n",buf);
	/*char c;
	while(read(sock,&c,1)>0)putc(c,stdout);
	*/
	printf("waiting for statistics\n\n");fflush(stdout);	
	while(read(sock,buf,MSGSIZE) <0);//oso diavazei 0 byte h an ginetai error sto read

	int bytes,files;
	float mesos_oros;
	double diaspora;
	int error_conn;
	sscanf(buf,"%d %d %f %lf %d",&bytes,&files,&mesos_oros,&diaspora,&error_conn);
	printf("bytesTransferred : %d || filesTransferred : %d , mesos_oros: %lf diaspora: %f  error_conn : %d\n\n",bytes,files,mesos_oros,diaspora,error_conn);
	
	close ( sock ) ;
	free(ContentServers);
}


void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}