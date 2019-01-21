xử;ý dữ liệu thông điệp gửi lên từ client
{			  //2
	int mark; // save type of string from client
	int i;
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
	// return code of statement is sent by client
	mark = splitString(in);
	if (mark == 0)
	{
		strcpy(in, "40");
	}
	else if (mark == 1)
	{
		strcpy(str, in);
		if (state == 2)
			strcpy(in, "14");
		else
		{ // state ==1 or 0 not signin
			cur = search(in);
			if (cur != NULL)
			{
				if (cur->status == 0)
				{
					strcpy(in, "10");
					state = 0;
				}
				else
				{
					strcpy(in, "11");
					state = 1;
				}
			}
			else
			{
				strcpy(in, "12");
				state = 0;
			} // not found
		}
	}
	else if (mark == 2)
	{
		if (state != 1)
		{
			strcpy(in, "20");
		}
		else
		{ // waiting pass xxx
			cur = search(str);
			if (!strcmp(cur->pass, in))
			{
				cur->status = 3;
				state = 2;
				strcpy(in, "22");
			}
			else
			{
				cur->status--;
				strcpy(in, "21");
				if (cur->status == 0)
				{
					strcpy(in, "23");
					saveInfo();
					state = 0;
				}
			}
		}
	}
	else if (mark == 3)
	{
		if (state != 2)
			strcpy(in, "31");
		else
		{
			strcpy(in, "30");
		}
		state = 0;
	}
	strcpy(out, in);
	strcpy(in, "\0");
}


---> trả về mã 1 ,2 ,3 báo hiệu rằng đang làm gì
	char head[100], cont[100];
	int space_amount = 0, i, j;
	for (i = 0; i < strlen(recv); i++)
	{
		if (recv[i] == ' ')
			space_amount++;
	}
	if (space_amount > 1)
		return 0;
	if (space_amount == 0)
		if (!strcmp(recv, "logout"))
			return 3;
		else
			return 0;
	// space_amount ==1;
	strcpy(head, "");
	strcpy(cont, "");
	for (i = 0; i < strlen(recv); i++)
	{
		if (recv[i] != ' ')
			head[i] = recv[i];
		else
		{
			head[i] = '\0';
			break;
		}
	}
	for (j = i + 1; j < strlen(recv); j++)
	{
		cont[j - i - 1] = recv[j];
	}
	cont[j - i - 1] = '\0';
	strcpy(recv, cont);
	if (!strcmp(head, "user"))
		return 1;
	else if (!strcmp(head, "pass"))
		return 2;
	else
		return 0;

--> Xử lý tín hiệu bên client.c
		if (!strcmp(buff, "40"))
			printf("Not understand .\n Hint : Type 'user xxx' or 'pass xxx' or 'logout'\n");
		else if (!strcmp(buff, "20"))
			printf("Error state\n");
		else if (!strcmp(buff, "10"))
			printf("Account is blocked\n");
		else if (!strcmp(buff, "11"))
			printf("+OK\n");
		else if (!strcmp(buff, "12"))
			printf("User not found\n");
		else if (!strcmp(buff, "13"))
			printf("Account is signed\n");
		else if (!strcmp(buff, "14"))
			printf("Having a account is signed\n");
		else if (!strcmp(buff, "21"))
			printf("Pass not true\n");
		else if (!strcmp(buff, "22"))
			printf("Pass true . Account is signed\n");
		else if (!strcmp(buff, "23"))
			printf("Account is block because 3 times typed pass not true\n");
		else if (!strcmp(buff, "30"))
			printf("Logout success\n");
		else if (!strcmp(buff, "31"))
			printf("Logout fail . Not signed\n");
		else
			printf("--%s--\n", buff);  