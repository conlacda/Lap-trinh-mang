/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc!= 2){
        printf("Usage: %s <port_number> \n",argv[0]);
        return 1;
    }

	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
    int i=0,j=0;
    char buff1[BUFF_SIZE],buff2[BUFF_SIZE];
	//Step 1: Construct a UDP socket
	if ((server_sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}
	
	//Step 2: Bind address to socket
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */   
	bzero(&(server.sin_zero),8); /* zero the rest of the structure */

  
	if(bind(server_sock,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}     
	
	//Step 3: Communicate with clients
	while(1){
		sin_size=sizeof(struct sockaddr_in);
    		
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);
		
		if (bytes_received < 0)
			perror("\nError: ");
		else{
			buff[bytes_received] = '\0';
			printf("[%s:%d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
			// printf("%s\n",buff);
		}
		

        // Solve the data from client server
        int n;
        
        for (n=0;n<strlen(buff);n++){
        	if ((buff[n]>='a' && buff[n]<'z') || (buff[n]>='A' && buff[n]<'Z')){
                buff1[i++] = buff[n];
        	} else 
        	if (buff[n]>='0' && buff[n]<='9') buff2[j++] = buff[n];
        	  else if (buff[n]!=' ' && buff[n]!='\n')
        	  	bytes_sent = sendto(server_sock, "Error\n", 5, 0, (struct sockaddr *) &client, sin_size );    	  
        }
        buff1[i]='\0'; buff2[j]='\0';
        if (i!=0){
        	if (j!=0) { buff1[i]='\n';buff1[i+1]='\0';} 
        }
        strcat(buff1,buff2);  
        i=0;j=0;
		bytes_sent = sendto(server_sock, buff1, bytes_received, 0, (struct sockaddr *) &client, sin_size );
		if (bytes_sent < 0)
			perror("\nError: ");					
	}
	close(server_sock);
	return 0;
}
