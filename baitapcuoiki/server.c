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
#include "question.c"
#include "team.c"
#include "solvestring.c"

#define BACKLOG 20 /* Number of allowed connections */
#define BUFF_SIZE 1024
#define MAX_LEN 100

typedef struct node
{
	char name[MAX_LEN];
	char pass[MAX_LEN];
	int status;
	int signin;
	struct node *next;
} userInfo;
userInfo *root, *cur, *new;

int state = 0;
char str[MAX_LEN]; // user_name is typed

/* The processData function copies the input string to output */
void processData(int sockfd ,char *in, char *out, char errorCode[]);

/* The recv() wrapper function*/
int receiveData(int s, char *buff, int size, int flags);

/* The send() wrapper function*/
int sendData(int s, char *buff, int size, int flags);

/* Get list of user info*/
void getUserInfo(FILE *f);

/* Save file*/
void saveInfo();

userInfo *search(char str[]);

int splitString(char recv[]);

void buyWeaponForAttacking(int connfd, int weapon_number, char errorCode[]);

void buyItemForDefending(int connfd, int item_number, int castle_number, char errorCode[]);

void attackCastle(int i, int castle_number, char errorCode[]);

void getTeamInfo(int i, char team_info[]);

void getCastleInfo(char castle_info[]);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage %s <port_number>\n", argv[0]);
		return 1;
	}
	int i, maxi, maxfd, listenfd, connfd, sockfd,j;
	int nready, client[FD_SETSIZE];
    char errorCode[MAX_LEN] = "";
	ssize_t ret;
	fd_set readfds, allset;
	char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{ /* calls bind() */
		perror("\nError: ");
		return 0;
	}

	//Step 3: Listen request from client
	if (listen(listenfd, BACKLOG) == -1)
	{ /* calls listen() nghe tại socket hiện tại với 20 (BACKLOG) kết nối tối đa*/
		perror("\nError: ");
		return 0;
	}

	maxfd = listenfd; /* initialize */
	maxi = -1;		  /* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1; /* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	// Đọc dữ liệu ra để chuẩn bị cho request từ máy khách
	FILE *f = fopen("account.txt", "r+");
	getUserInfo(f);
	loadQuestion();
	initTeam();
	initWeaponItem();
	//Step 4: Communicate with clients
	while (1)
	{
		readfds = allset;										/* structure assignment */
		nready = select(maxfd + 1, &readfds, NULL, NULL, NULL); // kiếm tra xem có bao nhiêu fd sẵn sàng đọc (readfds)
		if (nready < 0)
		{
			perror("\nError: ");
			return 0;
		}

		// khi có 1 client kết nối sinh ra 1 connfd và điều chỉnh các tham số
		if (FD_ISSET(listenfd, &readfds))
		{ /* new client connection */ // kiểm tra listenfd đã nằm trong fd_set *readfds chưa
			clilen = sizeof(cliaddr);
			if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) // chấp nhận cliaddr kết nối tới cổng
				perror("\nError: ");
			else
			{
				// printf("%d",connfd);  phân biêt 2 client với số connfd sinh ra
				printf("You got a connection from %s with connfd= %d\n", inet_ntoa(cliaddr.sin_addr), connfd); /* prints client's IP */
				if (splitTeam(connfd) != -1) // nếu team chưa đầy																   // nếu ko xếp đc đội thì đóng kết nối đó
				{
					printf("Thuoc team : %d\n",getTeamNumber(connfd));
					state = 0;
					for (i = 0; i < FD_SETSIZE; i++)
						if (client[i] < 0)
						{
							client[i] = connfd; /* save descriptor */
							break;
						}
					if (i == FD_SETSIZE)
					{
						printf("\nToo many clients");
						close(connfd);
					}
					// Gửi dữ liệu câu hỏi về cho người dùng khi kết nối tới
					for (i = 0; i < AMOUNT_OF_SMALL_QUESTION; i++)
					{
						sendData(connfd, strcat(small_question[i].content, "/"), strlen(small_question[i].content) + 1, 0);
						sendData(connfd, strcat(small_question[i].answer1, "/"), strlen(small_question[i].answer1) + 1, 0);
						sendData(connfd, strcat(small_question[i].answer2, "/"), strlen(small_question[i].answer2) + 1, 0);
						sendData(connfd, strcat(small_question[i].answer3, "/"), strlen(small_question[i].answer3) + 1, 0);
						sendData(connfd, strcat(small_question[i].answer4, "/"), strlen(small_question[i].answer4) + 1, 0);
					}
					// thêm vào dsach , xử lý tràn
					FD_SET(connfd, &allset); /* add new descriptor to set */
					if (connfd > maxfd)
						maxfd = connfd; /* for select */
					if (i > maxi)
						maxi = i; /* max index in client[] array */

					if (--nready <= 0)
						continue; /* no more readable descriptors */
				}
				else // team đầy
				{
					sendData(connfd, "400", 4, 0); 
					close(connfd);
				}
			}
		}

		// 1 vòng lặp quét qua dsach client[] để nhận và xử lý lần lượt dũ liệu gửi lên
		for (i = 0; i <= maxi; i++)
		{ /* check all clients for data */
			if ((sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &readfds))
			{
				ret = receiveData(sockfd, rcvBuff, BUFF_SIZE, 0);
				if (ret <= 0)
				{ // xóa client khỏi dsach đang hoạt động
					FD_CLR(sockfd, &allset);
					close(sockfd);
					client[i] = -1;
				}
				else
				{
					processData(sockfd,rcvBuff, sendBuff, errorCode);
					if (!strcmp(sendBuff,"220") || !strcmp(sendBuff,"221") || !strcmp(sendBuff,"222") || !strcmp(sendBuff,"223") ||
					!strcmp(errorCode,"210") || !strcmp(errorCode,"1"))
					    sendData(sockfd, sendBuff, strlen(sendBuff), 0);
					else sendData(sockfd,errorCode,strlen(errorCode),0);
					strcpy(sendBuff,""); strcpy(errorCode,""); //
					// 0 : flag cờ báo  ret kích thước mảng
					if (ret <= 0)
					{ // bao loi
						FD_CLR(sockfd, &allset);
						close(sockfd);
						client[i] = -1;
					}
					saveInfo();
				}
				if (--nready <= 0)
					break; /* no more readable descriptors */
			}
		}
	}
	return 0;
}

void buyWeaponForAttacking(int connfd, int weapon_number, char errorCode[]) {
    int i = getTeamNumber(connfd);

    if (weapon_number == 0) {
        if (team[i].resources[0] < 200 || team[i].resources[2] < 1500) {
            strcpy(errorCode, "241");
        }
		else {
			team[i].weapon = weapon_number;
			team[i].resources[0] -= 200;
			team[i].resources[2] -= 1500;
			strcpy(errorCode, "240");
		}
    }

	else if (weapon_number == 1) {
		if (team[i].resources[0] < 300 || team[i].resources[1] < 1500 || team[i].resources[2] < 400) {
            strcpy(errorCode, "241");
        }
		else {
			team[i].weapon = weapon_number;
			team[i].resources[0] -= 300;
			team[i].resources[1] -= 1500;
			team[i].resources[2] -= 400;
			strcpy(errorCode, "240");
		}
	}

	else if (weapon_number == 2) {
		if (team[i].resources[0] < 1800 || team[i].resources[1] < 2500 || team[i].resources[2] < 500) {
            strcpy(errorCode, "241");
        }
		else {
			team[i].weapon = weapon_number;
			team[i].resources[0] -= 1800;
			team[i].resources[1] -= 2500;
			team[i].resources[2] -= 500;
			strcpy(errorCode, "240");
		}
	}

	else {									// khong tim thay weapon
		strcpy(errorCode, "242");
	}
}

void buyItemForDefending(int connfd, int castle_number, int item_number, char errorCode[]) {
    int i = getTeamNumber(connfd);

	if (item_number > 4 || castle_number > 3) {				// khong tim thay item hoac lau dai
		strcpy(errorCode, "313");
	}
	else {
		if (team[i].owned_castle[castle_number] == 0) {		// khong so huu lau dai nay
			strcpy(errorCode, "312");
		}
		else {
			if (item_number == 0) {
				if (team[i].resources[1] < 50 || team[i].resources[2] < 200) {
					strcpy(errorCode, "311");
				}
				else {
					team[i].item = item_number;
					team[i].resources[1] -= 50;
					team[i].resources[2] -= 200;
					strcpy(errorCode, "310");
				}
   			}

			else if (item_number == 1) {
				if (team[i].resources[0] < 100 || team[i].resources[1] < 100 || team[i].resources[2] < 1000) {
					strcpy(errorCode, "311");
				}
				else {
					team[i].item = item_number;
					team[i].resources[0] -= 100;
					team[i].resources[1] -= 100;
					team[i].resources[2] -= 1000;
					strcpy(errorCode, "310");
				}
			}

			else if (item_number == 2) {
				if (team[i].resources[0] < 200 || team[i].resources[1] < 1000 || team[i].resources[2] < 200) {
					strcpy(errorCode, "311");
				}
				else {
					team[i].item = item_number;
					team[i].resources[0] -= 200;
					team[i].resources[1] -= 1000;
					team[i].resources[2] -= 200;
					strcpy(errorCode, "310");
				}
			}

			else if (item_number == 3) {
				if (team[i].resources[0] < 1000 || team[i].resources[1] < 2000 || team[i].resources[2] < 1000) {
					strcpy(errorCode, "311");
				}
				else {
					team[i].item = item_number;
					team[i].resources[0] -= 1000;
					team[i].resources[1] -= 2000;
					team[i].resources[2] -= 1000;
					strcpy(errorCode, "310");
				}
			}
		}
	}
}

void attackCastle(int i, int castle_number, char errorCode[]) {
	int defend_strong, k;

	for (k = 0; k < AMOUNT_OF_TEAM; k++) {
		if (team[k].owned_castle[castle_number] == 1) {
			defend_strong = array_item[team[k].item].item_strong;
			break;
		}
	}
	if (array_weapon[team[i].weapon].weapon_strong >= defend_strong) {
		strcpy(errorCode, "230");
		team[k].owned_castle[castle_number] = 0; // k : team bị chiếm
		// team[i].owned_castle[castle_number] = 1; // i : team đi chiếm
	}
	else {
		strcpy(errorCode, "231");
	}
}

void getTeamInfo(int i, char team_info[]) {
	char temp[MAX_LEN] = "";
	strcat(team_info, "Resource: \n");
	strcat(team_info, "Iron: ");
	sprintf(temp, "%d", team[i].resources[0]);
	strcat(team_info, temp);
	strcat(team_info, "\nStone: ");
	sprintf(temp, "%d", team[i].resources[1]);
	strcat(team_info, temp);
	strcat(team_info, "\nWood: ");
	sprintf(temp, "%d", team[i].resources[2]);
	strcat(team_info, temp);
	strcat(team_info, "\nGold: ");
	sprintf(temp, "%d", team[i].resources[3]);
	strcat(team_info, temp);
	strcat(team_info, "\nCastle status (0 - not holding, 1 - holding): ");
	for (int j = 0; j < 3; j++) {
		sprintf(temp, "%d", team[i].owned_castle[j]);
		strcat(team_info, temp);
		strcat(team_info, "/");
	}
}

void getCastleInfo(char castle_info[]) {
	int j, k;
	char temp[MAX_LEN] = "";
	for (j = 0; j < 3; j++) {
		int check = 0;

        strcat(castle_info, "Castle ");
        sprintf(temp, "%d", j);
        strcat(castle_info, temp);
        strcat(castle_info, ": \n");

		
		for (k = 0; k < AMOUNT_OF_TEAM; k++) {
			if (team[k].owned_castle[j] == 1) {
				check = 1;
				break;
			}
		}
		if (check == 0) {
			strcat(castle_info, "No team is holding!\n");
		}
		else {
			strcat(castle_info, "Team is holding: ");
			sprintf(temp, "%d", k);
			strcat(castle_info, temp);
			strcat(castle_info, "\nItem is defending: ");
			strcat(castle_info, array_item[team[k].item].item_name);
			strcat(castle_info, "\nDefending strong: ");
			sprintf(temp, "%d", array_item[team[k].item].item_strong);
			strcat(castle_info, temp);
			strcat(castle_info, "\n");
		}
	}
}

// return code of statement is sent by client
// INPUT : String from client  (in[])
// OUTPUT : Status/code for sending to client (out[])
void processData(int sockfd,char in[], char out[],char errorCode[])
{  
	int mark; // save type of string from client
	int i,j;
	char param1[100],param2[100];
	// delete ' ' and '\n' in end of string rcvBuff from client
	for (i = 0; i <= strlen(in); i++)
	{
		if (in[i] == '\n')
		{
			in[i] = '\0';
			i = strlen(in);
		}
	}
	i = i - 2;
	for (i; i >= 0; i--)
	{
		if (in[i] != ' ')
			break;
		else
			in[i] = '\0';
	}
	char commandCode[MAX_LEN];
	strcpy(commandCode, getCommandCode(in));

	i = getTeamNumber(sockfd);
	//TODO
	// xử lý 3 tham số của thông điệp gửi toi
	if (strcmp(commandCode, "BUYATT") == 0) {
		buyWeaponForAttacking(sockfd, atoi(getParameter1(in)), errorCode);
	} 

	else if (strcmp(commandCode, "BUYDEF") == 0) {
		buyItemForDefending(sockfd, atoi(getParameter1(in)), atoi(getParameter2(in)), errorCode);
	}

	else if (strcmp(commandCode, "ATTACK") == 0) {
		int castle_number = atoi(getParameter1(in));

		if (castle_number > 2) {
			strcpy(errorCode, "233");
		}

		else if (team[i].owned_castle[castle_number] == 1) {
			strcpy(errorCode, "232");
		}

		else {
			int check = 0;
			for (int k = 0; k < AMOUNT_OF_TEAM; k++) {
				if (team[k].owned_castle[castle_number] == 1) {
					check = 1;
					break;
				}
			}

			if (check == 0) {
				strcpy(errorCode, "234");
			}
			else {
				attackCastle(i, castle_number, errorCode);
			}
		}
	}

	else if (strcmp(commandCode, "GET") == 0) {
		if (atoi(getParameter1(in)) == 1) {
			char team_info[10*MAX_LEN] = "";
			getTeamInfo(i, team_info);
			strcpy(out, team_info);
			strcpy(errorCode, "210");
		}
		
		else if (atoi(getParameter1(in)) == 2) {
			char castle_info[10*MAX_LEN] = "";
			getCastleInfo(castle_info);
			strcpy(out, castle_info);
			strcpy(errorCode, "210");
		}
		
		else strcpy(errorCode,"404");
		
	} else
    if (strcmp(getCommandCode(in),"SHOWQT") == 0){
        i = atoi(getParameter1(in))-7;
		if (0>i || i>2) strcpy(out,"223");
		else {
			// ghép câu hỏi lại để gửi qua client
			// strcpy(out,"");
			char temp[100];
			strcpy(temp,""); 
		    
			strcpy(out,strcat(big_question[i].content,"/"));
	        strcpy(temp,strcat(big_question[i].answer1,"/"));		
			strcpy(out,strcat(out,temp));
			strcpy(temp,strcat(big_question[i].answer2,"/"));
			strcpy(out,strcat(out,temp));
			strcpy(temp,strcat(big_question[i].answer3,"/"));
			strcpy(out,strcat(out,temp));
			for (j=strlen(out);j>0;j--){
				if (out[j]!='/') out[j]='\0'; else break;
			}
			strcpy(temp,strcat(big_question[i].answer4,"/"));
			strcpy(out,strcat(out,temp));
		}
		strcpy(errorCode,"1");
	}
	else
	if (strcmp(getCommandCode(in),"ANSRESOURCE")==0){ // sovle answer question for resouces
        strcpy(param1,getParameter1(in));
		strcpy(param2,getParameter2(in));
		if (!strcmp(param2,"")) strcpy(out,"221") ;else { 
		    i = atoi(param1);
    		strcpy(out,param2);
	    	if (strlen(param2)>1) strcpy(out,"221"); else
	    	if (team[getTeamNumber(sockfd)].owned_resource[i]==1) strcpy(out,"222");
	    	else
	    	{
                if (strcmp(param2,"a")==0) j=1; 
	    		if (strcmp(param2,"b")==0) j=2; 
	    		if (strcmp(param2,"c")==0) j=3; 
		    	if (strcmp(param2,"d")==0) j=4; 
			    if (small_question_true_answer[i] == j) {
			    	strcpy(out,"220");
			    	team[getTeamNumber(sockfd)].owned_resource[i] = 1; 
		    	} else strcpy(out,"221");
		    }
		}	
	} 
    else 
	if (strcmp(getCommandCode(in),"ANSCASTLE") == 0){ 
        strcpy(param1,getParameter1(in));
		strcpy(param2,getParameter2(in));
		int i1 = atoi(param1)-7;
    	strcpy(out,param2);
        int v;
		
		if (param2[0] != 'a' && param2[0] != 'b' && param2[0] != 'c' && param2[0] != 'd') strcpy(errorCode,"404"); else{	
	    	if (strlen(param2)>1) strcpy(out,"221"); else
	    	if (team[getTeamNumber(sockfd)].owned_resource[i1]==1) strcpy(out,"222");
	    	else
	    	{
                if (strcmp(param2,"a")==0) j=1; 
	    		if (strcmp(param2,"b")==0) j=2; 
	    		if (strcmp(param2,"c")==0) j=3; 
		    	if (strcmp(param2,"d")==0) j=4; 
			    if (big_question_true_answer[i1] == j) {
			     
		            for ( v=0;v<3;v++){ // for (int v=0;v<AMOUNT_OF_TEAM;v++)
					    if (team[v].owned_castle[i1] == 1 ) strcpy(errorCode,"235");
					}
					if (!strcmp(errorCode,"235")) { }else {
						strcpy(out,"220");
			    	    team[getTeamNumber(sockfd)].owned_castle[i1] = 1; 
					} 
		    	} else strcpy(out,"221");
		    }
		} 
	}
	  else { // ko xử lý đc đều trả về 404 
		  strcpy(out,"404");
		  strcpy(errorCode,"404");
	  }
	// tách xâu xem tín hiệu gửi về 
}
int splitString(char recv[])
{ //1

}
int receiveData(int s, char *buff, int size, int flags)
{
	int n;
	n = recv(s, buff, size, flags);
	if (n < 0)
		perror("Error: ");
	return n;
}

int sendData(int s, char *buff, int size, int flags)
{
	int n;
	n = send(s, buff, size, flags);
	if (n < 0)
		perror("Error: ");
	return n;
}
// đọc dữ liệu người dùng từ file account.txt
void getUserInfo(FILE *f)
{
	char c, s[100];
	int mark = 1, count = 0;
	root = (userInfo *)malloc(sizeof(userInfo));
	cur = (userInfo *)malloc(sizeof(userInfo));
	new = (userInfo *)malloc(sizeof(userInfo));
	root->next = NULL;
	cur = root;
	while (1)
	{
		c = getc(f);
		if (c == '\n' || feof(f))
		{
			mark = 1;
			s[count] = '\0';
			count = 0;
			new->status = atoi(s);
			if (new->status == 1)
				new->status = 3;
			new->signin = 0;
			new->next = NULL;
			if (strcmp(new->name, ""))
			{
				cur->next = new;
				cur = new;
				new = (userInfo *)malloc(sizeof(userInfo));
			}
		}
		else if (c == ' ')
		{ // ? 2 dau cach if s != "" -> mark++ ...
			s[count] = '\0';
			count = 0;
			if (mark == 1)
				strcpy(new->name, s);
			else if (mark == 2)
				strcpy(new->pass, s);
			mark++;
		}
		else
		{
			s[count++] = c;
		}
		if (feof(f))
			break;
	}
}

void saveInfo()
{
	FILE *f;
	f = fopen("account.txt", "w");
	cur = (userInfo *)malloc(sizeof(userInfo));
	cur = root;
	while (cur->next != NULL)
	{
		cur = cur->next;
		if (strcmp(cur->name, ""))
		{
			fprintf(f, "%s ", cur->name);
			fprintf(f, "%s ", cur->pass);
			if (cur->status)
				fprintf(f, "%d\n", 1);
			else
				fprintf(f, "%d\n", 0);
		}
	}
	fclose(f);
	cur = (userInfo *)malloc(sizeof(userInfo));
}

userInfo *search(char str[])
{
	cur = root;
	while (cur != NULL)
	{
		if (!strcmp(cur->name, str))
			return cur;
		cur = cur->next;
	}
	return NULL;
}

// get type of string from client and return value to in[]
// INPUT : string
// OUTPUT : type of string from client (login or logout or fail)

/* luồng thực hiện 
Nhận dữ liệu với recieveData 
Xử lý với processData 
connfd : đánh dấu các client khác nhau 
các hàm getUserInfo , saveInfo ,.. để đọc dữ liệu vào cho hàm processData
--> Ý tưởng : hàm gửi dữ liệu về client đã đánh dấu đc 2 client khác nhau
nk state thì ko phân biệt đc 

Xử lý thông điệp mà người dùng gửi lên 
--> nếu là login , logout --> purpose =1
           Cướp tài nguyên ( trả lời câu hỏi , công thành , trang bị)
		   phòng thủ ( mua đồ)
*/