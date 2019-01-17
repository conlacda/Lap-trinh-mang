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