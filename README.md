# WHT 1.31 — bản port đang phát triển

Source độc lập dựa trên WHT / WHT-1.29, dành cho **x86** Warcraft III 1.31.1.12164 và 1.31.1.12173. Đây chưa phải bản đạt đầy đủ tính năng của WHT-1.29.

## Build và chạy

Chạy `build-1.31.cmd` để build Release, chạy tests và cài vào hai bản sao bên dưới. Yêu cầu Visual Studio C++ Win32, CMake và .NET 10. `WHT.ini` đang có được giữ nguyên; file tool cũ được backup và file mới được kiểm tra SHA-256.

- `runtime-test/Warcraft3-1.31.1.12164/Run-WHT-1.31.cmd`
- `runtime-test/Warcraft3-1.31.1.12173/Run-WHT-1.31.cmd`
- `runtime-test/Warcraft3-1.31.1.12164/Run-WHT-1.31-x64.cmd`
- `runtime-test/Warcraft3-1.31.1.12173/Run-WHT-1.31-x64.cmd`

Đã copy đầy đủ hai client, gồm Data/CASC, x86, x86_64 và các thành phần đi kèm. Tool chạy **x86**. Không copy WHT.mix vào thư mục gốc của người dùng. Installer và launcher từ chối đường dẫn ngoài runtime-test của project; installer bỏ qua client đang chạy. Đóng client test rồi chạy lại installer nếu cần cập nhật.

1.31 không có Miles/mss32 như 1.29, nên dùng `Run-WHT-1.31.cmd` để cài build hiện tại và nạp DLL. Mở trực tiếp `Warcraft III.exe` không nạp tool. Launcher tự tìm đúng client test đang chạy để attach; nếu WHT đã nạp thì không nạp trùng. Muốn nhận DLL mới cần đóng game rồi chạy lại script. DLL xác minh SHA-256 của EXE và chữ ký các hàm native theo từng build trước khi cài hook luồng UI.

## Phần đã triển khai

- Profile riêng cho 12164 / 12173: CGameUI, CCameraWar3, CFloatVariable và các hàm frame/font/texture native.
- Menu native dùng danh mục tám tab của WHT, ngôn ngữ, phím menu F8, cấu hình camera/FogZ, memory và macro. Các mục chưa port được khóa.
- Camera distance / far clipping qua hàm native, không sửa trang mã được game bảo vệ. Camera được tìm bằng RTTI vtable; giá trị được đọc qua getter native vì 1.31 mã hóa dữ liệu này.
- Macro ghi/phát/dừng/sửa; Ctrl+Shift+F9/F10/F12/F11 hoặc phím trong INI.
- Lệnh `#cam`, `#zoom`, `#fogz`, `#tool`, `#menu`, `#commandsettings`, `#full`, `#lock`, `#showmem`, các lệnh macro.
- Helper LoadFileMapWar3/session và updater riêng `toolinfo-1.31`, kiểm tra ProductName `WHT 1.31`, không chọn payload 1.29.

Gameplay cần phiên đăng nhập hợp lệ, mode Local và trận đang mở. `#fogz` ở đây là camera far clipping, chưa phải hệ thống scene fog. Chat command hiện nhận phím Enter/WM_CHAR, chưa có hook trạng thái chat native.

Runtime mount MPQ trong WHT.mix bằng API archive/provider native 1.31 và kiểm tra các tên font/icon/transparency được game giải quyết về đúng `CArchiveMpq` của tool trước khi dựng menu. Không thay icon hoặc texture trong suốt bằng hình stock, không đổi registry. Build cũng trích xuất 20 asset nguyên bản sang `dist/client-assets`; bản sao `WHT-assets` được giữ để kiểm tra/fallback. Font `fonts_VN.ttf` có glyph tiếng Việt; alpha của `Transparent.tga` bằng 0. Không nạp Storm.dll helper vào game; nó chỉ chạy trong chương trình trích xuất riêng.

## Chưa hoàn tất / chưa xác nhận

Chưa port các nhóm resource, t?y ch?nh thanh HP, MTH, time boss, auto skill/item, player/network và các chức năng khác không được mở trong menu; các header `*129` chỉ là tài liệu tham khảo và không được đưa vào runtime 1.31. x86_64 chưa hỗ trợ.

Build, helper self-tests và 2/2 CTest đã pass. Nạp DLL, chữ ký profile, callback luồng UI, session, mount MPQ/provider và camera readback đã pass trên cả hai client sao chép. Người dùng đã xác nhận #cam hoạt động trên cả 12164/12173, và button/dropdown/icon hết lem sau sửa asset/backdrop. Font/texture dùng đúng MPQ WHT; cờ visible, EdgeSize/inset và adapter widescreen dùng layout riêng 1.31. Macro, các hành vi chuyển trận và nhóm gameplay còn khóa chưa được đánh dấu pass. Computer-use đọc được cửa sổ nhưng chụp War3 timeout; visual pass ở lượt này dựa trên xác nhận người dùng.

Xem [PORT131.md](PORT131.md) và `analysis/build131-final.log`, `analysis/deployment-*.json`, log `x86/WHT.log` của từng client. Gói build nằm trong `dist/client-x86`; source có thể đóng bằng `package-source.ps1`.


## Ghi ch? x64

Source x64: `native/profile64.hpp`, `version64.cpp`, `gameplay64.hpp`. Profile ???c x?c minh t? memory image/RTTI/disassembly ri?ng cho m?i EXE x64, kh?ng d?ng offset x86. ?? n?i menu/font/texture, camera/#cam/#fogz, cooldown, HP/s?MP/s, mana bar, HP/Mana values/max, level hero v? n?n gi?n theo s?; macro/menu config/memory d?ng ph?n chung. C?c nh?m ch?a port c?a x86 v?n kh?a ? x64. C?c l?nh c?a s? `#full/#lock` v? updater x64 ch?a n?i; kh?ng t? ?p payload updater x86 l?n x64.

Build x64, ki?m tra ki?n tr?c PE c?a DLL/launcher v? tests con tr?/v?ng ??i frame/geometry pass. Hai b?n x64 ?? ???c c?i v?i backup, preserve INI v? hash, launcher n?p DLL. Menu/MPQ c?a build n?n x64 ?? c? log native pass tr?n 12173; build gameplay m?i v?n c?n ki?m tra trong tr?n tr??c khi ??nh d?u ch?c n?ng hi?n th? pass. Computer-use tr? `window capture timed out` sau retry; kh?ng d?ng ?? ?? suy ra visual pass.
