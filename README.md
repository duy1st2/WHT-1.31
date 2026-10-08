# WHT cho Warcraft III 1.29

Dự án riêng cho **1.29.2.9231 (1.29b), x86**. Mã nguồn, assets, MinHook,
thư mục build và gói phát hành đều nằm trong `WHT-1.29`, không dùng chung
file với `WHT` của 1.24/1.26.

- Mở `build-1.29.cmd` để build, kiểm tra và cài vào hai client 1.29.
- Build không cài: `powershell -ExecutionPolicy Bypass -File build.ps1 -WithTests`.
- Kết quả: `build-wht/Release/WHT.mix`, `mss32.dll`, `WHT.dll`.
- Bản phát hành: `dist/client`. Client thử nghiệm: `runtime-test/Warcraft3-129`.
- Game vẫn dùng tên `WHT.mix`, `WHT.ini`, `WHT.log`, `WHT.macro.txt` như cũ.
- Giữ `WHT_MSS32_Original.dll` của game; không đổi INI/log/macro khi cập nhật.
- Client đang chạy sẽ được bỏ qua khi cài, không bị đóng tự động.

Tiến độ chức năng: [PORT129.md](PORT129.md). `#nfe` đang tạm bỏ qua theo yêu cầu.
Các thay đổi tương lai cho 1.29 thực hiện trong `native/version129.cpp` và
các header tại đây. Dự án `../WHT` chỉ build 1.24/1.26.
