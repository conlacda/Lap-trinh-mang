#include <stdio.h>          /* Login Logout in TCP */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

typedef struct node{
	char name[100];
	char pass[100];
	int  status;
    int  signin;
    struct node *next;
} userInfo;
userInfo *root,*cur,*new;

// Input : FILE *f 
// Read data in file account.txt and save in a linked list
void getUserInfo(FILE *f) { 
    char c,s[100]; 
    int mark = 1 , count = 0;
    root = (userInfo*) malloc(sizeof(userInfo));
    cur  = (userInfo*) malloc(sizeof(userInfo));
    new  = (userInfo*) malloc(sizeof(userInfo));
    root->next = NULL;
    cur = root;
    while (1){
        c = getc(f);
        if (c == '\n' || feof(f) ){
          mark = 1;
          s[count] = '\0';
          count = 0; 
          new->status = atoi(s);
          if (new->status == 1) new->status = 3;
          new->signin = 0;
          new->next = NULL ;
          if (strcmp(new->name,"")){
              cur->next = new;
              cur = new ;
              new = (userInfo*) malloc(sizeof(userInfo));
          } 
        } else if (c == ' '){ // ? 2 dau cach if s != "" -> mark++ ...  
          s[count] = '\0';
          count = 0;
            if (mark == 1) strcpy(new->name,s);
              else if (mark ==2) strcpy(new->pass,s);
            mark ++;
        } else {
          s[count++] = c ;
        }
        if (feof(f)) break;
    }
}

// Input : a string is sent from client
// Output : a integer value   1 : "user xxx"  2 : "pass xxx"  3 : logout  4 : exception
int splitString(char recv[]){
    char head[100],cont[100];
    int i=0,j;
    while (recv[strlen(recv)]==' ') recv[strlen(recv)] = '\0'; //
    while (recv[i]==' ') i++;//
    for (j=i;j<strlen(recv);j++){
        if (recv[j] == ' ') {
        	head[j-i]='\0';
        	break;
        } else
        head[j-i] = recv[j];
    }
   
    i = j+1;
    for (j=i;j<strlen(recv)-1;j++){
    	if (recv[j] == ' ') return 0;
    	cont[j-i] = recv[j];
    }
    cont[j-i] = '\0';
    strcpy(recv,cont);
    if ( (!strcmp(head,"user")) && (strcmp(recv,""))) return 1;
    else if ((!strcmp(head,"pass")) && (strcmp(recv,""))) return 2;
    else if (!strcmp(head,"logout\n")) return 3;
    return 0;
}

// 0~not exist  1~found  2~blocked
// Input : a string - name of account
// Output : pointer of account with name = string OR Null when not found
userInfo* search(char str[]){
	cur = root;
	while (cur!=NULL){
		if (!strcmp(cur->name,str)) return cur;
		cur = cur->next;
	}
    return NULL;
}

// save the linked list to acount.txt file
void saveFile(){
	FILE *f;
	f = fopen("account.txt","w");
    cur = root;
    while (cur->next != NULL){
       cur = cur->next;
       if (strcmp(cur->name,"")){
           fprintf(f, "%s ",cur->name); 
           fprintf(f, "%s ",cur->pass );
           if (cur->status) fprintf(f, "%d\n",1); else fprintf(f,"%d\n",0);
       }
    }
    fclose(f);
    cur  = (userInfo*) malloc(sizeof(userInfo));
}
int main(int argc, char *argv[])
{
    if (argc!= 2){
    	printf("Usage %s <port_number>\n",argv[0]);
		return 1;
    }
	int listen_sock, conn_sock,state,mark; /* file descriptors */
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

	FILE *f = fopen("account.txt","r+");
	getUserInfo(f);

	// //Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

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
			// Solve data and send response
            // state = 2 da co nguoi dang nhap
            mark = splitString(recv_data);
            if (mark == 1) { // typed user "x"
                if (state ==2) strcpy(recv_data,"14");
                else { 
                	cur = search(recv_data);
                    if (cur != NULL){
                        if (cur->status == 0) strcpy(recv_data,"10");
                        else if (cur->signin) strcpy(recv_data,"13");
                            else {
                                strcpy(recv_data,"11");
                                state = 1; // "user xxx" was sent by client to server
                            } 
                    } 
                    else 
                        strcpy(recv_data,"12");
                }

            }
            else {
            	if (mark == 0) { 
            	    strcpy(recv_data,"40"); // fail
            	}
                else 
                	if (mark==2) {  // 
                        if (state != 1) { strcpy(recv_data,"20"); state =0;}
                        else  // type pass after typed a user name
                        	if (!strcmp(cur->pass,recv_data)) {
                        		cur->signin = 1;
                        		strcpy(recv_data,"22");
                        		state = 2; // signin success
                        		cur->status = 3;
                        	} 
                        else { // pass not true
                            cur->status--;
                            strcpy(recv_data,"21");
                            if (cur->status == 0) {
                            	strcpy(recv_data,"23");
                                saveFile();
                            	state = 0; // logout or not sign in
                            }
                        }
                    }
                else if (mark ==3){
                	if (state!=2) strcpy(recv_data,"31");
                    else {
                    	strcpy(recv_data,"30");
                        cur->signin = 0;
                    }
                    state = 0;
                    
                }    
            }
            
			//echo to client
			bytes_sent = send(conn_sock, recv_data, bytes_received, 0); /* send to the client welcome message */
			if (bytes_sent <= 0){
				printf("\nConnection closed");
				break;
			}
		}//end conversation
        saveFile();
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}