#include <stdio.h>
#include <string.h>
typedef struct quest{
    char content[100];
    char answer1[100];
    char answer2[100];
    char answer3[100];
    char answer4[100];
    int true_answer;
    int stt;
}question;
#define AMOUNT_OF_SMALL_QUESTION 18  // 6 bai *3 cau hoi 
#define AMOUNT_OF_BIG_QUESTION 3     // 3 bai * 2 cau hoi
question small_question[18],big_question[3]; /* small_question : câu hỏi cho bãi khai thác 
                                               , big_question : câu hỏi cho lâu đài */
char small_question_content[AMOUNT_OF_SMALL_QUESTION][100] ={
    "con lợn có mấy chân 0 ?  ", 
    "con gà có mấy mỏ 1?",
    "con lợn có mấy chân 2? ",
    "con gà có mấy mỏ 3?",
    "con lợn có mấy chân 4? ",
    "con gà có mấy mỏ 5?",
    "con lợn có mấy chân 6? ",
    "con gà có mấy mỏ 7?",
    "con gà có mấy mỏ 8?",
    "con gà có mấy mỏ 9?",
    "con gà có mấy mỏ 10?",
    "con gà có mấy mỏ 11?",
    "con gà có mấy mỏ 12?",
    "con gà có mấy mỏ 13?",
    "Con gì thân nhất với con người?",
    "Mùa nào lạnh nhất ?",
    "Cơ thể con người chứa gì nhiều nhất?",
    "con gà sống ở đâu 17?",
};
char small_question_answer[AMOUNT_OF_SMALL_QUESTION*4][100] ={
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "con chó","con gà","con lợn","con bò",
    "mùa xuân","mùa hè","mùa thu","mùa đông",
    "nước","xương","tóc","móng tay",
    "mặt đất","dưới biển","trên trời","lòng đất",
};
// trắc nghiêm nên câu trả lời sẽ là 1,2,3,4
int small_question_true_answer[AMOUNT_OF_SMALL_QUESTION] = {
    4,1,2,3,3,1,2,3,4,1,2,3,3,1,1,4,1,1
};
char big_question_content[AMOUNT_OF_BIG_QUESTION][100] = { 
    "Căn bậc 2 của 25 là bao nhiêu?",
    "Giá trị của biểu thức 1+1 là ?",
    "Một bàn tay có mấy ngón?",
};
char big_question_answer[AMOUNT_OF_BIG_QUESTION*4][100] ={
    "10","7","5","6",
    "1","2","3","4",
    "5","2","3","4",
};
int big_question_true_answer[AMOUNT_OF_BIG_QUESTION] = {
    3,2,1
};

// int team_big_question[AMOUNT_OF_TEAM][AMOUNT_OF_BIG_QUESTION] ; // lưu team nào đã trả lời đc câu hỏi nào
// Load Questions by server
void loadQuestion(){ 
    int  i;
    for (i=0;i<AMOUNT_OF_BIG_QUESTION;i++){
        strcpy(big_question[i].content,big_question_content[i]);
        strcpy(big_question[i].answer1,big_question_answer[4*i]);
        strcpy(big_question[i].answer2,big_question_answer[4*i+1]);
        strcpy(big_question[i].answer3,big_question_answer[4*i+2]);
        strcpy(big_question[i].answer4,big_question_answer[4*i+3]);
        big_question[i].true_answer = big_question_true_answer[i];
        
        // printf("%s\n",big_question[i].content);
        // printf("%s  %s\n",big_question[i].answer1,big_question[i].answer2);
        // printf("%s  %s\n",big_question[i].answer3,big_question[i].answer4);
        // printf("%d\n",big_question[i].true_answer);
    }
    for (i=0;i<AMOUNT_OF_SMALL_QUESTION;i++){
        strcpy(small_question[i].content,small_question_content[i]);
        strcpy(small_question[i].answer1,small_question_answer[4*i]);
        strcpy(small_question[i].answer2,small_question_answer[4*i+1]);
        strcpy(small_question[i].answer3,small_question_answer[4*i+2]);
        strcpy(small_question[i].answer4,small_question_answer[4*i+3]);
        small_question[i].true_answer = small_question_true_answer[i];        
    }
    printf("Question is loaded . Server is ready \n");
}
void sendQuestion(){ // gửi về cho máy khách

}
void recvQuestion(){

}// nhận lần lượt từng câu 1

void displayQuestion(int x){ // x tu 0-5 chi ra do la bai khai thac , 6-8 chi ra do la lau dai
    int i;
    if (0<=x && x<=5 ){ 
        for (i=0;i<3;i++){
            if (i==0) printf("Question for iron (key = %d)\n",3*x+i);
            else if (i==1) printf("Question for stone (key = %d)\n",3*x+i);
            else if (i==2) printf("Question for wood (key = %d)\n",3*x+i);
                printf("Question : %s \n",small_question[3*x+i].content);
                printf("Answer : \n ");
                printf("a, %s\n",small_question[3*x+i].answer1);
                printf("b, %s\n",small_question[3*x+i].answer2);
                printf("c, %s\n",small_question[3*x+i].answer3);
                printf("d, %s\n",small_question[3*x+i].answer4);
        }
    } else 
    if (6<=x && x<=8)
    {
        printf("Question of castle (key = %d ) \n",x+1);
        printf("Question : %s \n",big_question[x-6].content);
        printf("Answer : \n");
        printf("a, %s\n",big_question[x-6].answer1);
        printf("b, %s\n",big_question[x-6].answer2);
        printf("c, %s\n",big_question[x-6].answer3);
        printf("d, %s\n",big_question[x-6].answer4);
    } else printf("Answer not found");
}
