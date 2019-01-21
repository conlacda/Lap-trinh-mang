// đăng nhập đăng xuất với các tài khoản đã có sẵn trong file
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024

typedef struct node{
	char name[100];
	char pass[100];
	int status;
	int signin;
	struct node *next;
} userInfo;
userInfo *root,*cur,*new;

int state =0;
char str[100]; // user_name is typed

/* The processData function copies the input string to output */
void processData(char *in, char *out);

/* The recv() wrapper function*/
int receiveData(int s, char *buff, int size, int flags);

/* The send() wrapper function*/
int sendData(int s, char *buff, int size, int flags);

/* Get list of user info*/
void getUserInfo(FILE *f);

/* Save file*/
void saveInfo();

userInfo* search(char str[]);

int splitString(char recv[]);

int main(int argc, char *argv[])
{
	if (argc!=2){
		printf("Usage %s <port_number>\n",argv[0]);
		return 1;
	}
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t	ret;
	fd_set	readfds, allset;
	char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	} 

	//Step 3: Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

    FILE *f = fopen("account.txt","r+");
    getUserInfo(f);

	maxfd = listenfd;			/* initialize */
	maxi = -1;				/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	
	//Step 4: Communicate with clients
	while (1) {
		readfds = allset;		/* structure assignment */
		nready = select(maxfd+1, &readfds, NULL, NULL, NULL);
		if(nready < 0){
			perror("\nError: ");
			return 0;
		}
		
		if (FD_ISSET(listenfd, &readfds)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0)
				perror("\nError: ");
			else{
				printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
				state = 0;
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						break;
					}
				if (i == FD_SETSIZE){
					printf("\nToo many clients");
					close(connfd);
				}

				FD_SET(connfd, &allset);	/* add new descriptor to set */
				if (connfd > maxfd)
					maxfd = connfd;		/* for select */
				if (i > maxi)
					maxi = i;		/* max index in client[] array */

				if (--nready <= 0)
					continue;		/* no more readable descriptors */
			}
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &readfds)) {
				ret = receiveData(sockfd, rcvBuff, BUFF_SIZE, 0);
				if (ret <= 0){
					FD_CLR(sockfd, &allset);
					close(sockfd);
					client[i] = -1;
				}
				else {
					// printf("%ld\n",strlen(rcvBuff));
					processData(rcvBuff, sendBuff);
					sendData(sockfd, sendBuff, ret, 0);
					if (ret <= 0){ // bao loi
						FD_CLR(sockfd, &allset);
						close(sockfd);
						client[i] = -1;
					}
					saveInfo();
				}
				if (--nready <= 0)
					break;		/* no more readable descriptors */
			}
		}
	}
	
	return 0;
}
// return code of statement is sent by client
// INPUT : String from client  (in[])
// OUTPUT : Status/code for sending to client (out[])
void processData(char in[], char out[]){ //2
	int mark ; // save type of string from client
	int i;
	// delete ' ' and '\n' in end of string rcvBuff from client
    for (i=0;i<=strlen(in);i++){
    	if (in[i]=='\n') { in[i]='\0'; i=strlen(in);}
    }
    i=i-2;
    for (i;i>=0;i--){
        if (in[i]!=' ')    
        	break;
        else in[i] = '\0';
    }    
    // return code of statement is sent by client
    mark = splitString(in);
    if (mark ==0){
    	strcpy(in,"40");
    } else 
    if (mark ==1){
    	strcpy(str,in);
        if (state ==2) strcpy(in,"14");
        else { // state ==1 or 0 not signin
            cur = search(in);
            if (cur!=NULL){
            	if (cur->status ==0) {strcpy(in,"10"); state =0;}
            	else { strcpy(in,"11"); state =1;}
            } else {strcpy(in,"12"); state =0;} // not found
        } 
    } else 
    if (mark ==2){
       if (state !=1) {
           strcpy(in,"20");
       } else { // waiting pass xxx
           cur = search(str);
           if (!strcmp(cur->pass,in)){
           	   cur->status = 3;
           	   state =2;
           	   strcpy(in,"22");
           } else {
           	   cur->status --;
           	   strcpy(in,"21");
           	   if (cur->status == 0){
           	   	   strcpy(in,"23");
           	   	   saveInfo();
           	   	   state = 0;
           	   }
           }
       }
    } else
    if (mark ==3){
    	if (state!=2) strcpy(in,"31");
    	else{
    		strcpy(in,"30");
    	}
    	state =0;
    }
    strcpy(out,in);
    strcpy(in,"\0");
}

int receiveData(int s, char *buff, int size, int flags){
	int n;
	n = recv(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

int sendData(int s, char *buff, int size, int flags){
	int n;
	n = send(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

void getUserInfo(FILE *f){
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

void saveInfo(){
	FILE *f;
	f = fopen("account.txt","w");
	cur = (userInfo*) malloc(sizeof(userInfo));
	cur = root;
	while (cur->next!=NULL){
		cur = cur->next;
		if (strcmp(cur->name,"")){
			fprintf(f, "%s ",cur->name);
			fprintf(f, "%s ",cur->pass);
			if (cur->status) fprintf(f,"%d\n",1); else fprintf(f,"%d\n",0);
		}
	}
	fclose(f);
	cur = (userInfo*) malloc(sizeof(userInfo));
}

userInfo* search(char str[]){
	cur = root;
	while (cur!=NULL){
		if (!strcmp(cur->name,str)) return cur;
		cur = cur->next;
	}
    return NULL;
}

// get type of string from client and return value to in[]
// INPUT : string
// OUTPUT : type of string from client (login or logout or fail)
int splitString(char recv[]){ //1
    char head[100],cont[100];
    int space_amount=0,i,j;
    for (i=0;i<strlen(recv);i++){
    	if (recv[i]==' ') space_amount++;
    }
    if (space_amount>1) return 0;
    if (space_amount==0) 
    	if (!strcmp(recv,"logout")) return 3; else return 0;
    // space_amount ==1;
    strcpy(head,""); strcpy(cont,"");
    for (i=0;i<strlen(recv);i++){
        if (recv[i] !=' ') head[i]=recv[i]; else {
            head[i] ='\0';
        	break;
        }
    }
    for (j=i+1;j<strlen(recv);j++){
        cont[j-i-1] = recv[j];
    }
    cont[j-i-1] = '\0';
    strcpy(recv,cont);
    if (!strcmp(head,"user")) return 1;
    else if (!strcmp(head,"pass")) return 2;
    else return 0;
}