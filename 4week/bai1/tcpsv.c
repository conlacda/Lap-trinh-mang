#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

// solve data from client  
// INPUT : a string from client sent OUTPUT : a solved string  
void solveData(char buff[BUFF_SIZE]){
    char buff1[BUFF_SIZE],buff2[BUFF_SIZE];
    int i=0,j=0,n;
    for (n = 0; n < strlen(buff) ; n++){
    	if ((buff[n]>='a' && buff[n]<='z') || (buff[n]>='A' && buff[n]<='Z')){
                buff1[i++] = buff[n];
        	} else 
        	if (buff[n]>='0' && buff[n]<='9') buff2[j++] = buff[n];
        	  else if (buff[n]!=' ' && buff[n]!='\n'){
                  strcpy(buff,"Error\n");
                  return ;
        	  } 
    }
    buff1[i]='\0'; buff2[j]='\0';
    if (i!=0){
        if (j!=0) { buff1[i]='\n';buff1[i+1]='\0';} 
    }
    strcat(buff1,buff2);    	  	  	  
    strcpy(buff,buff1);
    return;
}

int main(int argc, char *argv[])
{
    if (argc!= 2){
		printf("Usage %s <port_number>\n",argv[0]);
		return 1;
	}

	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1){
			//receives message from client
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				recv_data[bytes_received] = '\0';
				printf("\nReceive: %s ", recv_data);
			}
			solveData(recv_data);
			//echo to client
			if (!strcmp(recv_data,"Error\n")) 
			    bytes_sent = send(conn_sock, recv_data, 5, 0);
			else	
			    bytes_sent = send(conn_sock, recv_data, bytes_received, 0); /* send to the client welcome message */
			if (bytes_sent <= 0){
				printf("\nConnection closed");
				break;
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}