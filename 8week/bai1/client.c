#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[]){
    struct iovec  iov[2];

	if (argc!= 3){
		printf("Usage %s <ip> <port_number>\n",argv[0]);
		return 1;		
	}
	int client_sock;
	char buff[BUFF_SIZE + 1];
	char str1[BUFF_SIZE + 1];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	while(1){

	//send message
	printf("\nInsert string to send:");
	memset(buff,'\0',(strlen(buff)+1));
	fgets(buff, BUFF_SIZE, stdin);		
	msg_len = strlen(buff);
	if (msg_len ==0) break;
	if (!strcmp(buff,"\n")) break;	
	bytes_sent = send(client_sock, buff, msg_len, 0);
	if(bytes_sent < 0)
		perror("\nError: ");
	
	strcpy(buff,"");
	strcpy(str1,"");
    iov[0].iov_base = buff;
    iov[1].iov_base = str1;
    iov[0].iov_len = BUFF_SIZE;
    iov[1].iov_len = BUFF_SIZE;

    bytes_received = readv(client_sock,iov,2);
    strcpy(buff,(char*)iov[0].iov_base);
    if (buff[0] == 'E' && buff[1] == 'r' && buff[2] == 'r' && buff[3] == 'o' && buff[4] == 'r') printf("Error\n"); else
    printf("%s\n%s",(char*)iov[0].iov_base,(char*)iov[1].iov_base);	
    }	 
	//Step 4: Close socket
	close(client_sock);
	return 0;
}