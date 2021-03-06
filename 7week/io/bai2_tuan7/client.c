// bài 6 : select serve + client ở bài trước
// broadcast : bài 8 ( mk nghỉ học ? )--> hẹn thời gian gửi tín hiệu
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[]){
	if (argc!= 3){
		printf("Usage %s <ip> <port_number>\n",argv[0]);
		return 1;		
	}
	int client_sock;
	char buff[BUFF_SIZE + 1];
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
	
	//receive echo reply
	bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
	if (bytes_received < 0)
			perror("\nError: ");
	else if (bytes_received == 0)
			printf("Connection closed.\n");
		
	buff[bytes_received] = '\0';
	if (!strcmp(buff,"40")) printf("Not understand .\n Hint : Type 'user xxx' or 'pass xxx' or 'logout'\n");
    else if (!strcmp(buff,"20")) printf("Error state\n");
    else if (!strcmp(buff,"10")) printf("Account is blocked\n");
    else if (!strcmp(buff,"11")) printf("+OK\n");
    else if (!strcmp(buff,"12")) printf("User not found\n");
    else if (!strcmp(buff,"13")) printf("Account is signed\n");
    else if (!strcmp(buff,"14")) printf("Having a account is signed\n");
    else if (!strcmp(buff,"21")) printf("Pass not true\n");
    else if (!strcmp(buff,"22")) printf("Pass true . Account is signed\n");
    else if (!strcmp(buff,"23")) printf("Account is block because 3 times typed pass not true\n");
    else if (!strcmp(buff,"30")) printf("Logout success\n");
    else if (!strcmp(buff,"31")) printf("Logout fail . Not signed\n");
    else printf("%s\n",buff);	
    }	 
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
