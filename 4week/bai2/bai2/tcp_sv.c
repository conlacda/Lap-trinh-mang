#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

// Cut name of file from the path of file in client
// Input : ex  D:/abc/a.txt  Output : /a.txt
void extractFilename(char filename[]){
	char *str;
	char str1[20];
	strcpy(str1,"/");
	strcat(str1,filename);
	str = strrchr(str1,'/');
	strcpy(filename,str);
}

//Check existence of file
//Input : File name  output : path in sever to save 
int existenceOfFile(char filename[]){
	extractFilename(filename);
    char str[BUFF_SIZE];
    strcpy(str,"server_storage");
    strcat(str,filename);
    FILE *f = fopen(str,"r+");
    if (f) return 1;
    strcpy(filename,str);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc!= 2){
    	printf("Usage %s <port_number>\n",argv[0]);
		return 1;
    }
	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE],filename[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size,file_size,count;
	FILE *f;

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
			// extractFilename(recv_data) ; ***
			if (existenceOfFile(recv_data)) bytes_sent = send(conn_sock, "Error: File is existent on server", 33, 0); 
			//echo to client
			else{
		    	bytes_sent = send(conn_sock,"Start receiving file", 20, 0); /* send to the client welcome message */
		    	strcpy(filename,recv_data);
		    	
		    	strcpy(recv_data,"\0");
		        recv(conn_sock,recv_data,BUFF_SIZE-1,0);
		        //recv_data : byte will be received
		        file_size = atoi(recv_data);
		        printf("%d\n",file_size);
                // receive the content of file
                while (file_size>0){
                	f = fopen(filename,"a+"); // create new file with name indentified
                	strcpy(recv_data,"");
                    bytes_received = recv(conn_sock,recv_data,BUFF_SIZE-1,0);
                	if (file_size-bytes_received<=0){
                        recv_data[file_size] = '\0';
                	} 
                	file_size-=bytes_received;
                	fwrite(recv_data,sizeof(recv_data)-1,1,f); 
                    fclose(f);
                    if (bytes_received==0) break;
                }    
		    }
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