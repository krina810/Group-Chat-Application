#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
	
#define BUFSIZE 1024
		
void send_recv(int i, int sockfd, char *cli_name)
{
	char send_buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;
//cli_name = argv[3];
	
	if (i == 0){
		fgets(send_buf, BUFSIZE, stdin);
	if (strcmp(send_buf , "quit\n") == 0) {
			exit(0);
		}else
		send(sockfd, send_buf, strlen(send_buf), 0);
	}else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		printf("%s\n" , recv_buf);
		fflush(stdout);
	}
}
		
		
void connect_request(int *sockfd, struct sockaddr_in *server_addr, int *argc, char *argv[], char *cli_name)
{

	struct hostent *server;
int port;
if (*argc < 4)
	{

	fprintf(stderr,"usage %s hostname port client_name\n", argv[0]);
	exit(0);

	}
port=atoi(argv[2]);
	
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
server = gethostbyname(argv[1]);

	if ( server==NULL)
	{
		perror("ERROR no such host \n");
		exit(0);
	}

	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(port);
if(inet_pton(AF_INET, argv[1], &server_addr->sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        exit(0);
    } 
	//server_addr->sin_addr.s_addr = INADDR_ANY;                            
	//server_addr->sin_addr.s_addr = inet_addr(server);
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);
	
	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}

}
	
int main(int argc, char *argv[])
{

	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;
	char *ip;
char *cli_name;
	
	connect_request(&sockfd, &server_addr, &argc, argv, cli_name);
	printf("%s connected\n", argv[3]);
	FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;
	
	while(1){

		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				send_recv(i, sockfd, cli_name);
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}
