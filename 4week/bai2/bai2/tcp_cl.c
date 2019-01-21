#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 8192
// Check Existence of FileName on client
// Input : Filename   Output : 0 ~ not exist  1 ~exist
int existenceOfFile(char filename[]){
	filename[strlen(filename)-1] = '\0';
	FILE *f;
    f = fopen(filename,"r+");
    if (!f) {
    	printf("Error: File not found");
    	return 0;
    }    
    return 1;
}

int main(int argc, char *argv[]){
	if (argc!= 3){
		printf("Usage %s <ip> <port_number>\n",argv[0]);
		return 1;		
	}
	int client_sock;
	char buff[BUFF_SIZE],filename[BUFF_SIZE],str[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received,size,count;
	FILE *f;
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
		if (msg_len == 0) break;
		if (!existenceOfFile(buff)) continue;   // check Exist of file
		strcpy(filename,buff);
		bytes_sent = send(client_sock, buff, msg_len, 0);
		if(bytes_sent <= 0){
			printf("\nConnection closed!\n");
			break;
		}
		
		//receive echo reply
		bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0); 
		if (!strcmp(buff,"Start receiving file")) {     // Ready to send file to server
            printf("Reply from server: %s", buff);
            f = fopen(filename,"rb");
            fseek(f, 0, SEEK_END); 
            size = ftell(f);         // get size of file
            fseek(f, 0, SEEK_SET);
            sprintf(str,"%d",size);             
            send(client_sock,str,strlen(buff),0); //send sizeof file to server
         
            //send the content of file
            count = 0;
            while (count<=(size/(BUFF_SIZE-1))){
            	strcpy(str,"");
            	if (count==(size/(BUFF_SIZE-1))) fread(str,size-count*(BUFF_SIZE-1),1,f);
            	else fread(str,BUFF_SIZE-1,1,f);
            	size-=sizeof(str);
            	send(client_sock,str,sizeof(str),0);  // send data stream
            	count++;
            }
            fclose(f);
            printf("Successful transfering\n");
            //
		}
		else if (!strcmp(buff,"Error: File is existent on server")) printf("Reply from server: %s", buff);
		if(bytes_received <= 0){
			printf("\nError!Cannot receive data from sever!\n");
			break;
		}
		
		buff[bytes_received] = '\0';
		// printf("Reply from server: %s", buff);
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}