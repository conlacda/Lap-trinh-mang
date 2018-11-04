#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node{
	char name[100];
	char pass[100];
	int  status;
    int  signin;
    struct node *next;
} userInfo;
userInfo *root,*cur,*new;

void displayMenu(){
	printf("USER MANAGEMENT PROGRAM \n--------------------------\n  1. Register \n  2. Sign in \n  3. Search \n  4. Sign out \nYour choice(1-4, other to quit) : ");
    return ;
}

userInfo* checkUserExist(char name[100]){ // search user with name and return userInfor if founded 
    cur = root;
    while (cur!= NULL){
    	if (!strcmp(cur->name,name)) {
    	  return cur;
    	}
    	cur = cur->next;
    }
    return NULL;
}

void Register(FILE *f){
	printf("--> Regist\n");
    char name[100],pass[100]; 
    printf("Username : ");
    scanf("%s",name);
    if (checkUserExist(name)!= NULL) printf("Account existed \n");
       else {
       	   printf("Password : ");
       	   scanf("%s",pass);
           cur = root;
           while (cur->next != NULL) cur = cur->next;
           new = (userInfo*) malloc(sizeof(userInfo));
           strcpy(new->name,name);
           strcpy(new->pass,pass);
           new->status = 3;
           new->next = NULL;
           cur->next = new;
           printf("Successful registration\n");
       };
}

void signin(){
    printf("--> Sign in\n");
    char name[100],pass[100];
    printf("Username : ");
    scanf("%s",name);
    cur = checkUserExist(name); 
    if (cur != NULL) {
    	if (cur->signin) { 
    		printf("Account is signed in\n");
    		return;
        }
    	if (cur->status == 0 ) {
    		printf("Account is blocked\n");
    		return;
    	}
        printf("Password : ");
        scanf("%s",pass);
        if (strcmp(cur->pass,pass)) {
        	printf("Password is incorrect . \n");
            cur->status--;
            if (!cur->status) {
            	printf("Account is blocked \n");
            }
        }
          else { 
          	  printf("Hello %s\n",cur->name);
              cur->signin = 1;
          }
    } else {
        printf("Cannot find account\n");   
    }
}

void search(){ 
	printf("--> Search\n");
	char name[100];
    printf("Username : ");
    scanf("%s",name);
    cur = checkUserExist(name);
    if (cur == NULL) { 
    	printf("Cannot find account\n");
        return ;
    }
    if (cur->status) {
        printf("Account is active\n");
    } else printf("Account is blocked\n");
}

void signout(){
	printf("--> Sign out\n");
	char name[100];
    printf("Username : ");
    scanf("%s",name);
    cur = checkUserExist(name);
    if (cur == NULL) {
    	printf("Cannot find account\n");
        return;
    } else{
    	if (cur->signin == 0) printf("Account is not sign in\n");
    	else {
    		printf("Good bye %s\n",cur->name);
    		cur->signin = 0;
    	}
    }
}

void getUserInfo(FILE *f) { // du lieu dang nguyen thuy do FILE *f ko bi thay doi gia tri
    char c,s[100]; 
    int mark = 1 , count = 0;
    root = (userInfo*) malloc(sizeof(userInfo));
    cur  = (userInfo*) malloc(sizeof(userInfo));
    new  = (userInfo*) malloc(sizeof(userInfo));
    root->next = NULL;
    cur = root;
       while (1){
        c = getc(f);
        if (feof(f)){
          break;
        }
        if (c == '\n'){
          mark = 1;
          s[count] = '\0';
          count = 0; 
          new->status = atoi(s);
          if (new->status == 1) new->status = 3;
          new->signin = 0;
          new->next = NULL ;
          cur->next = new;
          cur = new ;
          new = (userInfo*) malloc(sizeof(userInfo)); 
        } else if (c == ' '){ // ? 2 dau cach if s != "" -> mark++ ...  
          s[count] = '\0';
          count = 0;
            if (mark == 1) strcpy(new->name,s);
              else if (mark ==2) strcpy(new->pass,s);
            mark ++;
        } else {
          s[count++] = c ;
        }
    }
}

int main(){
	// char *s;
    FILE *f = fopen("account.txt","r+");
	if (!f) {
		printf("Cannot find the data file . Are you want create (y)?");
		char confirm ;
		scanf("%c",&confirm);
		if (confirm == 'y') {
			f = fopen("account.txt","a+");
		} else
		return 1;
    }
    getUserInfo(f);
    int choice = 1;
    while (choice >=1 && choice <= 4){
        displayMenu();
        scanf ("%d",&choice);
        switch (choice){
            case 1 : 
                Register(f);
                break;
            case 2 :
                signin();
                break;
            case 3 :
                search();
                break;
            case 4 : 
                signout();
                break;
            default : 
                f = fopen("account.txt","w");
                cur = root;
                while (cur->next != NULL){
                   cur = cur->next;
                   fprintf(f, "%s ",cur->name); 
                   fprintf(f, "%s ",cur->pass );
                   if (cur->status) fprintf(f, "%d\n",1); else fprintf(f,"%d\n",0);
                } 
                printf("Exit program\n");                
        }    
    }
    fclose(f);
    return 0;
}
