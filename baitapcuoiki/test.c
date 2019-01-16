#include <stdio.h>
#include <string.h>

#define MAX_LEN 100
// Get a string is command code after splitting received message
// parameter: a string is received message
// return a string which is command code
char* getCommandCode(char message[]) {
	char temp[MAX_LEN];
	char* commandCode;

	strcpy(temp, message);

	commandCode = strtok(temp, " ");

	return commandCode;
}

// Get a string is data of command message after splitting received message
// parameter: a string is received message
// return a string which is data of command message
char* getParameter1(char message[]) {
	char temp[MAX_LEN];
	char* dataMessage;

	strcpy(temp, message);

	dataMessage = strtok(temp, " ");
	dataMessage = strtok(NULL, " ");

	return dataMessage;
}

char* getParameter2(char message[]) {
	char temp[MAX_LEN];
	char* dataMessage;

	strcpy(temp, message);

	dataMessage = strtok(temp, " ");
	dataMessage = strtok(NULL, " ");
        dataMessage = strtok(NULL, " ");

	return dataMessage;
}

int main()
{
    char str[100];
    strcpy(str,"Cai         binh         dien        ");
    printf("-%s-",getParameter2(str));
    return 1;
}