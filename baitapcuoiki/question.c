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
#define AMOUNT_OF_BIG_QUESTION 3     // 3 bai * 1 cau hoi
question small_question[18],big_question[3]; /* small_question : câu hỏi cho bãi khai thác 
                                               , big_question : câu hỏi cho lâu đài */
char small_question_content[AMOUNT_OF_SMALL_QUESTION][100] ={
    "con lợn có mấy chân 0 ?                       ", 
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
    "con gà có mấy mỏ 14?",
    "con gà có mấy mỏ 15?",
    "con gà có mấy mỏ 16?",
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
    "1","2","3","4",
    "1","2","3","4",
    "1","2","3","4",
    "mặt đất","dưới biển","trên trời","lòng đất",
};
// trắc nghiêm nên câu trả lời sẽ là 1,2,3,4
int small_question_true_answer[AMOUNT_OF_SMALL_QUESTION] = {
    4,1,2,3,3,1,2,3,4,1,2,3,3,1,2,3,1,1
};
char big_question_content[AMOUNT_OF_BIG_QUESTION][100] = { 
    "Căn bậc 2 của 25 là bao nhiêu?",
    "Giá trị của biểu thức 1+1 là ?",
    "Một bàn tay có mấy ngón?" 
};
char big_question_answer[AMOUNT_OF_BIG_QUESTION*4][100] ={
    "10","7","5","6",
    "1","2","3","4",
    "5","2","3","4",
};
int big_question_true_answer[AMOUNT_OF_BIG_QUESTION] = {
    3,2,1,
};
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
        printf("Question is loaded\n");
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
}
void sendQuestion(){ // gửi về cho máy khách

}// ý tưởng : gửi lần lượt hết 1 câu rồi chuyển qua câu khác  
void recvQuestion(){

}// nhận lần lượt từng câu 1

void displayQuestion(int x){ // x tu 0-5 chi ra do la bai khai thac , 6-8 chi ra do la lau dai
    printf("%d",x);
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
        printf("Answer of castle");
        // TODO gửi lên server lấy trạng thái của lâu đài
    } else printf("Answer not found");
}
