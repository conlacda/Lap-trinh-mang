*Vấn đề : khi client kết nối tới server thì server phải gửi dữ liệu về số đội + câu hỏi
2 thứ này dính vào nhau khó gỡ
==> Giải pháp : ở đây client nhận được câu hỏi coi như là đã thiết lập thành công
Người dùng có thể xem thông tin qua lênh get ( request-response tuần tự để tránh bị dính thông điệp)

số câu hỏi được đánh từ 0 đến 17 nhưng do SHOWANS x --> khi x là char x được tính là 0
                                          SHOWANS UNKNOW <--> SHOWANS 0
số câu hỏi sẽ được -1 khi gửi đi đến serve  (client 1-18  server 0-17)           
--> hien thi cau hoi ra truoc mat nguoi dung
    Tra loi cau hoi len may chu -> phan hoi theo ma code

solveString.c : lỗi hàm lấy tham số thứ 2 khi thông điệp gửi chỉ có 1 lệnh + 1 tham số
--> ANSRESOURCE ko gửi 1 tham số ( core dumped server)

đã fix chia team
fix tra loi cau hoi co nguoi khac chiem

230 - tan cong chua dc giai ma 
team chiem cung chua dc thay doi

Kịch bản : 2 người vào 
--> ANSCASTLE 7 c : chiếm đc lâu đài  (A)
    ANSCASTLE 7 c --> (B) -- 235
    BUYDEF 0 1 (A)
    ATTACK 0 (B)
    BUYATT 1 (B)
    ATTACK 0 (B) : phá thành công
    ATTACK 0 (B) : không có người sở hữu 234
    ANSCASTLE 7 c (B) : chiếm đc thành
// BUYATT 1 : mua vũ khi
// ATTACK 0 : tấn công