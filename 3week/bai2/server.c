/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 1024
struct hostent *host;
struct in_addr **addr_list;
// Check input data from client is ip or domain name
// INPUT : a string   OUTPUT : 0 ~ IP , 1 ~ domain
int checkIporDomain(char buff[]){ 
    char a[3];
    buff[strlen(buff)-1] = '\0';
    int j,count=0,cs=0;
    for (j=0; j<strlen(buff);j++){
        if (buff[j]!='.') {
            if (buff[j]<'0' || buff[j]>'9') return 1;
            a[cs] = buff[j];
            cs++;
        }
            else {
                count++;
                if (count >=4) return 1;
                a[cs] = '\0';
                if (atoi(a)<0 || atoi(a)>255) return 1;
                cs = 0;
                a[cs]='\0';
            }
    }
    if (count!=3) return 1;
    a[3]= '\0';
    if (atoi(a)<0 || atoi(a)>255) return 1;
    return 0;
}
// Convert a domain name to IP(s)  
// INPUT : Domain name  OUTPUT : string IP has IP(s)
void convertDomaintoIp(char buff[]){
	int i;
    host = gethostbyname(buff);
    char buff1[BUFF_SIZE];
    if (gethostbyname(buff) == NULL){
    	strcpy(buff,"Not found information\n");
    	return;
    }
    addr_list = (struct in_addr **)host->h_addr_list;
    if (addr_list[0] != NULL ) {
    	strcpy(buff,"Official IP:");
    	strcat(buff,inet_ntoa(*addr_list[0]));
    }
    if (addr_list[1] != NULL) strcat(buff,"\nAlias IP: \n");
    for(i = 1; addr_list[i] != NULL; i++) {       
        strcat(buff, inet_ntoa(*addr_list[i]));
        strcat(buff,"\n");
    }
    return;
}
// Convert a IP to domain name(s)
// INPUT : an IP  OUTPUT : domain name(s)
void convertIptoDomain(char buff[]){
	char buff1[BUFF_SIZE];
    struct in_addr ipv4addr;
    inet_pton(AF_INET,buff, &ipv4addr);
    host = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    if(host == NULL) {
    	strcpy(buff,"Not found information");
        return ;
    }    	
    strcpy(buff,"Official name:");
    strcat(buff,host->h_name);
    return;
}

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
		}
  

        // Resend solved data to client
		if (checkIporDomain(buff))  convertDomaintoIp(buff);
			else convertIptoDomain(buff);	
	    bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *) &client, sin_size );
        if (bytes_sent < 0)
			perror("\nError: ");		
	}

	close(server_sock);
	return 0;
}
