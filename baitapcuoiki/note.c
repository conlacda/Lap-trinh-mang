*Vấn đề : khi client kết nối tới server thì server phải gửi dữ liệu về số đội + câu hỏi
2 thứ này dính vào nhau khó gỡ
==> Giải pháp : ở đây client nhận được câu hỏi coi như là đã thiết lập thành công
Người dùng có thể xem thông tin qua lênh get ( request-response tuần tự để tránh bị dính thông điệp)