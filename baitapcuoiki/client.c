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
	int client_sock, i, num;
	char buff[BUFF_SIZE + 1];
	char *token;
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
		if (strcmp(getCommandCode(buff), "SHOWQT") == 0)
		{									 // nếu tín hiệu ko gửi lên máy chủ
			num = atoi(getParameter1(buff)); // SHOWANS x : get question  thể hiện đây là bãi số mấy
			if (num == 0 || num > 9)
				printf("Unsiutable parameter (y/c : 1-9) 1->6 : resources & 7->9 : castles");
			else
			{ // display list of questions
				num--;
				if (0 <= num && num <= 5)
				{ // chỗ này tổng quát phải là AMOUNT_OF_SMALL_QUESTION/AMOUNT_OF_QUESTION_A_RESOURCE  (+AMOUNT_OF_SMALL_QUESTION/3)
					displayQuestion(num);
				}
				else if (6 <= num && num <= 8)
				{
					send(client_sock, buff, strlen(buff), 0);
					recv(client_sock, buff, BUFF_SIZE, 0);
					if (strcmp(buff, "223") == 0)
						printf("Error ! Question is not found");
					else
					{
						token = strtok(buff, "/");
						strcpy(big_question[num - 6].content, token);
						token = strtok(NULL, "/"); // đối số là NULL : quy định sẽ chuyển tới token kế tiếp chứ ko phải coi NULL là xâu như BUFF
						strcpy(big_question[num - 6].answer1, token);
						// printf("%s",token);
						token = strtok(NULL, "/");
						strcpy(big_question[num - 6].answer2, token);
						token = strtok(NULL, "/");
						strcpy(big_question[num - 6].answer3, token);
						token = strtok(NULL, "/");
						strcpy(big_question[num - 6].answer4, token);
						displayQuestion(num);
					}
				}
			}
		}
		else // ko phải thông điệp lấy câu hỏi --> chỉ trả về opcode
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
			if (!strcmp(buff,"220")) printf("Answer is true . Get resource of this castle");
			else if (!strcmp(buff,"221")) printf("Answer is false"); 
			else if (!strcmp(buff,"223")) printf("Question not found");
			else if (!strcmp(buff,"222")) printf("You owned this castle");
			else printf("%s",buff);
		}
		// printf("%s",buff);
	}
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
// strstr() trả về xâu a trong xâu b