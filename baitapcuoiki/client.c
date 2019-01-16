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
#include "question.c"
#include "solvestring.c"

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Usage %s <ip> <port_number>\n", argv[0]);
		return 1;
	}
	int client_sock,i,num;
	char buff[BUFF_SIZE + 1];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;

	//Step 1: Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	//Step 3: Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
	// Step 4 : Receive the list of question for "bai khai thac"
	
	// Nhận danh sách câu hỏi ngay lần đầu kết nối tới
	// thông điệp dạng a/b/c dính vào nhau
	bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
	buff[bytes_received] = '\0';
	if (strcmp(buff, "400") == 0)
		printf("Server is busy , please try later !!");
	else
	{
		printf("Connect successfully");
		char *token;
		token = strtok(buff, "/");
		int count_question = 0;
		while (token != NULL)
		{
			strcpy(small_question[count_question].content, token);
			token = strtok(NULL, "/"); // đối số là NULL : quy định sẽ chuyển tới token kế tiếp chứ ko phải coi NULL là xâu như BUFF
			strcpy(small_question[count_question].answer1, token);
			token = strtok(NULL, "/");
			strcpy(small_question[count_question].answer2, token);
			token = strtok(NULL, "/");
			strcpy(small_question[count_question].answer3, token);
			token = strtok(NULL, "/");
			strcpy(small_question[count_question].answer4, token);
			token = strtok(NULL, "/");
			count_question++;
		}
		// small_question[i] --> lưu lại các câu hỏi của 6 bãi khai thác
		// for (i=0;i<count_question;i++){ // show ra màn hình để kiểm tra câu hỏi gửi về có đúng ko?
		// 	printf("-%s-\n",small_question[i].content);
		// 	printf("%s\n",small_question[i].answer1);
		// 	printf("%s\n",small_question[i].answer2);
		// 	printf("%s\n",small_question[i].answer3);
		// 	printf("%s\n",small_question[i].answer4);
		// }
	}
	//Step 5: Communicate with server

	while (1)
	{
		//send message
		printf("\nInsert string to send:");
		memset(buff, '\0', (strlen(buff) + 1));
		fgets(buff, BUFF_SIZE, stdin);
		msg_len = strlen(buff);
		if (msg_len == 0)
			break;
		if (!strcmp(buff, "\n"))
			break;
		if (strcmp(getCommandCode(buff),"SHOWQT") == 0){ // nếu tín hiệu ko gửi lên máy chủ
            num = atoi(getParameter1(buff)); // SHOWANS x : get question  thể hiện đây là bãi số mấy
            if (num==0 || num>9) printf("Unsiutable parameter (y/c : 1-9) 1->6 : resources & 7->9 : castles");
			else {// display list of questions
			    num--;
				if (0<=num<=5) {} // chỗ này tổng quát phải là AMOUNT_OF_SMALL_QUESTION/AMOUNT_OF_QUESTION_A_RESOURCE  (+AMOUNT_OF_SMALL_QUESTION/3)
				else if (6<=num<=9){
                   // TODO nhận dữ liệu từ server cho trạng thái + câu hỏi 
				   sendData(client_sock,buff,strlen(buff),0);
				}
                displayQuestion(num);
			}
		}
		else // tín hiệu được gửi lên máy chủ
		{
			bytes_sent = send(client_sock, buff, msg_len, 0);
			if (bytes_sent < 0)
				perror("\nError: ");

			//receive echo reply
			bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
			if (bytes_received < 0)
				perror("\nError: ");
			else if (bytes_received == 0)
				printf("Connection closed.\n");

			buff[bytes_received] = '\0';
			// printf("%s",buff);
		} 
		// printf("%s",buff);
	}
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
// strstr() trả về xâu a trong xâu b