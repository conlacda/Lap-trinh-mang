#include <stdio.h>

#define MAX_LEN 100;

#define AMOUNT_OF_TEAM 3
#define AMOUNT_OF_CASTLE 3
#define AMOUNT_OF_RESOURCE 6
#define AMOUNT_OF_PEOPLE_A_TEAM 3
#define AMOUNT_OF_WEAPON 3
#define AMOUNT_OF_ITEM 4

// bui
typedef struct struct_weapon {
    int weapon_number;
    char weapon_name[MAX_LEN];
    int weapon_strong;
    int iron;
    int stone;
    int wood;
    int gold;
} struct_weapon;

typedef struct struct_item {
    int item_number;
    char item_name[MAX_LEN];
    int item_strong;
    int iron;
    int stone;
    int wood;
    int gold;
} struct_item;

struct_weapon array_weapon[3];

array_weapon[0].weapon_number = 0;
strcpy(array_weapon[0].weapon_name, "Ballista");
array_weapon[0].weapon_strong = 1000;
array_weapon[0].iron = 100;
array_weapon[0].stone = 100;
array_weapon[0].wood = 1000;
array_weapon[0].gold = 0;

array_weapon[1].weapon_number = 1;
strcpy(array_weapon[1].weapon_name, "Catapult");
array_weapon[1].weapon_strong = 3000;
array_weapon[1].iron = 200;
array_weapon[1].stone = 1000;
array_weapon[1].wood = 200;
array_weapon[1].gold = 0;

array_weapon[2].weapon_number = 2;
strcpy(array_weapon[2].weapon_name, "Ballista");
array_weapon[2].weapon_strong = 8000;
array_weapon[2].iron = 1000;
array_weapon[2].stone = 2000;
array_weapon[2].wood = 1000;
array_weapon[2].gold = 0;


struct_item array_item[AMOUNT_OF_ITEM];

array_item[0].item_number = 0;
strcpy(array_item[0].name, "Fence");
array_item[0].item_strong = 200;
array_item[0].iron = 0;
array_item[0].stone = 50;
array_item[0].wood = 200;
array_item[0].gold = 0;

array_item[1].item_number = 1;
strcpy(array_item[1].name, "Wood wall");
array_item[1].item_strong = 1000;
array_item[1].iron = 100;
array_item[1].stone = 100;
array_item[1].wood = 1000;
array_item[1].gold = 0;

array_item[2].item_number = 2;
strcpy(array_item[2].name, "Stone wall");
array_item[2].item_strong = 3000;
array_item[2].iron = 200;
array_item[2].stone = 1000;
array_item[2].wood = 200;
array_item[2].gold = 0;

array_item[0].item_number = 3;
strcpy(array_item[3].name, "Legend wall");
array_item[0].item_strong = 8000;
array_item[0].iron = 1000;
array_item[0].stone = 2000;
array_item[0].wood = 1000;
array_item[0].gold = 0;

// end bui

typedef struct tm{
    int team_number;
    int socket_addr[AMOUNT_OF_PEOPLE_A_TEAM]; // lưu tất cả địa chỉ của người vào 1 team
    int resources[4]; // iron , stone , wood , gold
    int passed_small_question[AMOUNT_OF_SMALL_QUESTION];
    int passed_big_question[AMOUNT_OF_BIG_QUESTION];
    int owned_castle[3]; // 0 or 1  3 bãi
    int owned_resource[6];
    int item;   // vật phẩm phòng ngự
    int weapon; // vũ khí
}struct_team;
struct_team team[AMOUNT_OF_TEAM]  ; // mặc định là có 3 team
// tổng quát : phải khởi tạo đội -> bao nhiêu người sẽ tạo ra bằng ấy đội

void initTeam(){
    int i=0,j;
    for (i=0;i<AMOUNT_OF_TEAM;i++){
        team[i].team_number = i;
        for (j=0;j<AMOUNT_OF_PEOPLE_A_TEAM;j++) 
            team[i].socket_addr[j]=0;
    }
}
// chia đội cho 1 người kết nối tới
int splitTeam(int connfdf){
    int i=0,j=0,flag=0; // flag : đánh dấu có tìm được team hay ko ?
    for (i=0;i<AMOUNT_OF_TEAM;i++){
        for (j=0;j<AMOUNT_OF_PEOPLE_A_TEAM;j++){
            if (team[i].socket_addr[j] ==0) { // nếu trong team có địa chỉ vẫn là 0 thì là còn chỗ trống
                flag = 1;  
                team[i].socket_addr[j]=connfdf; // chia đội cho địa chỉ connfd kết nối tới 
                return i;    // số đội     
            }
        }
    }
    return -1; // số đội đầy
}
// Kiểm tra xem người này thuộc team nào 
int getTeamNumber(int connfd){
    int i,j;
    for (i=0;i<AMOUNT_OF_TEAM;i++){
        for (j=0;j<AMOUNT_OF_PEOPLE_A_TEAM;j++)
        if (team[i].socket_addr[j]==connfd) return i;
    }
    return -1;
}

// chia đội chơi --> thêm 1 cấu trúc đội rồi gán socket_addr cho họ
// người chơi trả lời câu hỏi
/* 3 hàm get --> thông tin bãi khai thác 
                 thông tin lâu đài
                 thông tin cá nhân 
Giao thức sẽ được sử dụng để đáp ứng yêu cầu của client ( thế hiện trạng thái của request) 
 Client : thông điệp theo chuẩn
 Server : trả về 1 opcode 

 Gửi câu hỏi + chia đội : ko cần client request  --> ko dùng giao thức
*/