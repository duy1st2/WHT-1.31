#pragma once
// Tooltip text is kept separate from saved configuration keys and visible labels.
inline std::string MthActionHelp(int action){
 if(toolEnglish.load()){const char* en[]={"Craft Shield","Craft Ice","Craft Wild","Craft Frozen","Craft Phoenix","Pick up Goku","Chicken route 1","Chicken route 2","Chicken route 3","Wolf route 1","Wolf route 2","Wolf route 3"};if(action<0||action>=12)return {};return std::string(en[action])+"\n"+(action<5?"Run the MTH purchase and crafting sequence.":action==5?"Perform the MTH Goku pickup action.":"Select a Chicken/Wolf unit and run this route for the configured number of laps. Use #setga to assign a courier and run while selecting your hero.");}
 const wchar_t* names[]={L"Ép Shield",L"Ép Ice",L"Ép Wild",L"Ép Frozen",L"Ép Phoenix",L"Nhặt Goku",L"Chạy Gà 1",L"Chạy Gà 2",L"Chạy Gà 3",L"Chạy Sói 1",L"Chạy Sói 2",L"Chạy Sói 3"};
 if(action<0||action>=12)return {};
 return Utf8(std::wstring(names[action])+L"\n"+(action<5?L"Thực hiện chuỗi mua và ghép đồ tương ứng trong MTH.":action==5?L"Thực hiện thao tác nhặt Goku trong MTH.":L"Điều khiển đơn vị chạy tuyến tương ứng; số lệnh xếp hàng theo mức đã chọn."));
}
inline std::string MenuControlHelp(const ToolMenuControl& d){
 if(!wcscmp(d.key,L"Fog Click")&&toolEnglish.load())return "Fog Click alerts\nWarn about selecting a unit hidden from that player. Shared vision and duplicate alerts are checked.";
 if(!wcscmp(d.section,L"AutoHeal")||!wcscmp(d.section,L"AutoMana")){
  if(toolEnglish.load())return !wcscmp(d.key,L"Enabled")?"Automatically use registered potion types from any inventory slot for the selected living owned unit. Standard potion types work without registration. Pauses during chat, menus and other automation.":"Use a potion below this percentage (integer 1..99). HP attempts have priority; potion attempts share 1.5 seconds of spacing.";
  return Utf8(!wcscmp(d.key,L"Enabled")?L"T\u1ef1 t\u00ecm b\u00ecnh trong c\u1ea3 6 \u00f4 t\u00fai c\u1ee7a \u0111\u01a1n v\u1ecb \u0111ang ch\u1ecdn. B\u00ecnh chu\u1ea9n kh\u00f4ng c\u1ea7n \u0111\u0103ng k\u00fd.":L"D\u00f9ng b\u00ecnh khi th\u1ea5p h\u01a1n ng\u01b0\u1ee1ng n\u00e0y (1-99%). \u01afu ti\u00ean HP; kho\u1ea3ng ch\u1edd gi\u1eefa c\u00e1c l\u1ea7n th\u1eed l\u00e0 1,5 gi\u00e2y.");
 }

 if(!wcscmp(d.key,L"Reveal Invisibles"))return toolEnglish.load()?"Show invisible units\nToggle invisible unit display, sharing the state of #tanghinh.":Utf8(L"Hi\u1ec3n th\u1ecb t\u00e0ng h\u00ecnh\nB\u1eadt/t\u1eaft hi\u1ec3n th\u1ecb unit t\u00e0ng h\u00ecnh, c\u00f9ng tr\u1ea1ng th\u00e1i v\u1edbi #tanghinh.");
 if(!wcscmp(d.section,L"Graphics")&&!wcscmp(d.key,L"Detailed Stats Tooltip"))return toolEnglish.load()?"Detailed damage/armor tooltips\nHover over Damage to see attacks per second, attack interval, attack speed bonus and the map maximum. Hover over Armor to see numeric movement speed. Turning this off restores the original tooltip.":Utf8(L"Chi ti\u1ebft damage/armor\nTr\u1ecf chu\u1ed9t v\u00e0o Damage \u0111\u1ec3 xem s\u1ed1 \u0111\u00f2n/gi\u00e2y, th\u1eddi gian gi\u1eefa hai \u0111\u00f2n, bonus t\u1ed1c \u0111\u1ed9 \u0111\u00e1nh v\u00e0 m\u1ee9c t\u1ed1i \u0111a c\u1ee7a map. Tr\u1ecf v\u00e0o Armor \u0111\u1ec3 xem t\u1ed1c \u0111\u1ed9 di chuy\u1ec3n b\u1eb1ng s\u1ed1. T\u1eaft \u0111\u1ec3 hi\u1ec7n tooltip g\u1ed1c.");

 if(!wcscmp(d.key,L"Keyboard Scroll Speed")||!wcscmp(d.key,L"Mouse Scroll Speed"))return Utf8(L"T\u1ed1c \u0111\u1ed9 cu\u1ed9n camera\n0..700%; 100% m\u1eb7c \u0111\u1ecbnh. Nh\u1eadp gi\u00e1 tr\u1ecb r\u1ed3i Enter \u0111\u1ec3 l\u01b0u.");
 if(!wcscmp(d.key,L"Borderless"))return Utf8(L"C\u1eeda s\u1ed5 kh\u00f4ng vi\u1ec1n\nT\u1eaft \u0111\u1ec3 kh\u00f4i ph\u1ee5c c\u1eeda s\u1ed5 g\u1ed1c.");
 if(!wcscmp(d.key,L"Lock Cursor"))return Utf8(L"Kh\u00f3a chu\u1ed9t\nTh\u1ea3 kh\u00f3a khi game m\u1ea5t focus.");
 if(!wcscmp(d.key,L"Sound Start Game"))return Utf8(L"\u00c2m b\u00e1o b\u1eaft \u0111\u1ea7u tr\u1eadn\nB\u1eadt gi\u1eefa tr\u1eadn kh\u00f4ng ph\u00e1t \u00e2m thanh.");

 if(!wcscmp(d.key,L"Fast Menu"))return toolEnglish.load()?"Fast Menu\nAccelerate the three menu animation models. Native actions and completion callbacks remain in control.":Utf8(L"Fast Menu\nT\u0103ng t\u1ed1c animation c\u1ee7a ba model menu. Gi\u1eef thao t\u00e1c v\u00e0 callback g\u1ed1c.");
 if(!wcscmp(d.section,L"StateTag")&&!wcscmp(d.key,L"ShowHpValue"))return Utf8(L"S\u1ed1 HP tr\u00ean unit\nB\u1eadt/t\u1eaft s\u1ed1 HP ri\u00eang v\u1edbi mana. B\u1eadt HP/Mana value trong T\u1ed5ng qu\u00e1t tr\u01b0\u1edbc.");
 if(!wcscmp(d.section,L"StateTag")&&!wcscmp(d.key,L"FontSize"))return Utf8(L"C\u1ee1 ch\u1eef HP/Mana\nT\u1eeb 1 \u0111\u1ebfn 20; m\u1eb7c \u0111\u1ecbnh 4. C\u1eadp nh\u1eadt ch\u1eef v\u00e0 kho\u1ea3ng c\u00e1ch d\u00f2ng ngay.");
 if(!wcscmp(d.key,L"ModeCast"))return Utf8(L"Ch\u1ebf \u0111\u1ed9 thi tri\u1ec3n\nNormal: ch\u1ecdn m\u1ee5c ti\u00eau nh\u01b0 game. Quick Cast: thi tri\u1ec3n t\u1ea1i chu\u1ed9t khi nh\u1ea5n ph\u00edm g\u1ed1c. Ph\u00edm remap v\u00e0 Cast on Self \u0111\u01b0\u1ee3c \u01b0u ti\u00ean.");
 if(!wcscmp(d.key,L"Hide Option Menu"))return toolEnglish.load()?"Block game menu (F10)\nBlock unmodified F10 and the Warcraft Menu button. The configured tool-menu key keeps priority.":Utf8(L"Kh\u00f3a m\u1edf menu game (F10)\nCh\u1eb7n F10 v\u00e0 n\u00fat Menu c\u1ee7a Warcraft. Ph\u00edm m\u1edf menu tool v\u1eabn \u0111\u01b0\u1ee3c \u01b0u ti\u00ean.");
 std::wstring key=d.key,section=d.section,title,help;
 auto set=[&](const wchar_t* t,const wchar_t* h){title=t;help=h;};
 auto starts=[&](const wchar_t* s){return key.rfind(s,0)==0;};
 auto slot=[&](){auto p=key.find_first_of(L"0123456789");return p==std::wstring::npos?1:1+int(wcstol(key.c_str()+p,nullptr,10));};
 if(section==L"Quick Chat"){
  if(key==L"Enable")set(L"Bật chat nhanh",L"Bật hoặc tắt toàn bộ phím gửi tin nhắn đã lưu.");
  else {auto p=key.find_first_of(L"0123456789");auto n=key.substr(p);title=L"Tin nhắn nhanh số "+n;
   help=d.kind==MenuControlKind::Key?L"Nhấn phím đã gán để gửi nội dung ở dòng này.":d.kind==MenuControlKind::Clear?L"Xóa nội dung tin nhắn ở dòng này.":L"Nhập nội dung sẽ gửi khi nhấn phím tắt ở dòng này.";}
 }else if(section==L"SlotHotkey"||section==L"SlotHotkeyShop"||section==L"SlotHotkeyInv"){
  auto name=section==L"SlotHotkeyInv"?L"vật phẩm":section==L"SlotHotkeyShop"?L"học kỹ năng/cửa hàng/sách kỹ năng":L"kỹ năng";
  title=L"Phím tắt "+std::wstring(name);
  if(key==L"Enable")help=L"Bật hoặc tắt cả nhóm phím tắt. Giữ nguyên thiết lập của từng ô.";
  else {title+=L" — ô "+std::to_wstring(slot());
   if(starts(L"QuickCast"))help=L"Thi triển ngay tại vị trí chuột khi nhấn phím đã gán.";
   else if(starts(L"AutoTarget"))help=L"Dùng kỹ năng hoặc vật phẩm lên tướng của bạn khi nhấn phím đã gán.";
   else if(d.kind==MenuControlKind::Check)help=L"Bật hoặc tắt phím tắt của riêng ô này.";
   else help=L"Nhấn phím đã gán để kích hoạt nút ở ô này.";}
 }else if(section==L"MTHKEY"){
  if(key==L"MTH49Enable")set(L"Chế độ MTH 49",L"Bật hỗ trợ MTH 49 và tắt chế độ G17.");
  else if(key==L"G17Enable")set(L"Chế độ G17",L"Bật hỗ trợ G17 và tắt chế độ MTH 49.");
  else if(key==L"Noti")set(L"Thông báo MTH",L"Hiển thị thông báo khi thực hiện thao tác MTH.");
  else if(key==L"TimeBoss")set(L"Thời gian boss",L"Hiển thị lịch boss và đếm ngược hồi sinh boss Human/Undead theo thời gian trận.");
  else {int action=-1;const wchar_t* keys[]={L"Shield",L"Ice",L"Wild",L"Frozen",L"Phoenix",L"PickGoku",L"ChayGa1",L"ChayGa2",L"ChayGa3",L"ChaySoi1",L"ChaySoi2",L"ChaySoi3"};
   for(int i=0;i<12;++i)if(starts(keys[i])){action=i;break;}
   if(starts(L"AutoGaDraw"))action=5+int(key.back()-L'0');
   if(starts(L"AutoSoiDraw"))action=8+int(key.back()-L'0');
   auto text=Wide(MthActionHelp(action));auto split=text.find(L'\n');title=text.substr(0,split);help=split==std::wstring::npos?L"":text.substr(split+1);
   if(d.kind==MenuControlKind::Check){title=L"Hiện nút "+title;help=L"Hiện hoặc ẩn nút thao tác tương ứng trên màn hình trận đấu.";}
   if(d.kind==MenuControlKind::Choice){title=L"Số lệnh "+title;help=L"Chọn số lệnh di chuyển xếp hàng bằng Shift cho tuyến này.";}}
 }else if(starts(L"HeroF")){title=L"Chọn tướng số "+key.substr(5);help=L"Nhấn phím đã gán để chọn tướng; nhấn hai lần để đưa camera tới tướng.";
 }else {
  struct Entry{const wchar_t* key;const wchar_t* title;const wchar_t* help;};
  static const Entry entries[]={
   {L"AutoSkills_Enable",L"Tự động kỹ năng",L"Bật chuỗi kỹ năng. Trỏ vào kỹ năng rồi nhấn Ctrl+Alt+1..9 để gán bước cho đơn vị đang chọn."},
   {L"AutoSkills_Active",L"Phím chạy/dừng kỹ năng",L"Nhấn phím đã gán để bắt đầu hoặc dừng chuỗi kỹ năng của đơn vị đang chọn."},
   {L"AutoSkills_Delay",L"Độ trễ thi triển",L"Khoảng chờ giữa các lần thử dùng kỹ năng, tính bằng giây trong trận."},
   {L"AutoSkills_Range",L"Phạm vi tìm mục tiêu",L"Tìm kẻ địch còn sống, nhìn thấy và gần nhất trong phạm vi này."},
   {L"SellFast",L"Bán đồ nhanh",L"Trỏ chuột vào vật phẩm trong túi rồi nhấn phím đã gán để bán món đó tại cửa hàng gần đó."},
   {L"hko_command",L"Tạm tắt/khôi phục thiết lập",L"Tạm tắt camera, sương mù, màn hình rộng, cuộn và độ trễ đã chỉnh. Nhấn lại để khôi phục."},
   {L"hko_menu",L"Mở/đóng menu",L"Nhấn phím đã gán để mở hoặc đóng menu công cụ trong trận."},
   {L"hko_MinimizeMultiboard",L"Thu gọn bảng thông tin",L"Nhấn phím đã gán để thu gọn hoặc mở rộng bảng thông tin của bản đồ."},
   {L"AutoClickLeft",L"Tự động nhấn chuột trái",L"Nhấn phím đã gán để bật hoặc tắt việc nhấn chuột trái liên tục tại vị trí chuột."},
   {L"AutoClickRight",L"Tự động nhấn chuột phải",L"Nhấn phím đã gán để bật hoặc tắt việc nhấn chuột phải liên tục tại vị trí chuột."},
   {L"SetIdTarget",L"Gán đơn vị mục tiêu",L"Ghi nhận đơn vị đang chọn làm mục tiêu cho các thao tác hỗ trợ nhắm mục tiêu."},
   {L"ShowCooldown",L"Thời gian hồi kỹ năng",L"Hiển thị số giây hồi còn lại trên nút kỹ năng và vật phẩm."},
   {L"ShowRegenHP",L"Tốc độ hồi máu",L"Hiển thị lượng máu hồi mỗi giây trong bảng chỉ số."},
   {L"ShowRegenMana",L"Tốc độ hồi năng lượng",L"Hiển thị lượng năng lượng hồi mỗi giây trong bảng chỉ số."},
   {L"ShowMaxValues",L"Giá trị máu/năng lượng tối đa",L"Thêm giá trị tối đa sau giá trị hiện tại. Cần bật hiển thị giá trị máu/năng lượng."},
   {L"ShowManaValue",L"Giá trị năng lượng",L"Thêm số năng lượng bên cạnh số máu của đơn vị."},
   {L"ShowLevelHero",L"Cấp tướng",L"Hiển thị cấp của tướng trên màn hình trận đấu."},
   {L"Fog Click",L"C\u1ea3nh b\u00e1o Fog Click",L"B\u00e1o khi ng\u01b0\u1eddi ch\u01a1i ch\u1ecdn unit khu\u1ea5t t\u1ea7m nh\u00ecn c\u1ee7a h\u1ecd. C\u00f3 ki\u1ec3m tra chia s\u1ebb t\u1ea7m nh\u00ecn v\u00e0 ch\u1ed1ng l\u1eb7p c\u1ea3nh b\u00e1o."},
   {L"Show Hotkey In Panel",L"Hiện phím trên nút",L"Hiển thị phím tắt trên các nút kỹ năng và vật phẩm."},
   {L"Show Resource",L"Bảng tài nguyên",L"Hiển thị bảng vàng, gỗ và quân số của người chơi."},
   {L"Quick Drop Item",L"Thả đồ nhanh",L"Giữ Shift và nhấn chuột phải vào vật phẩm trong túi để thả nhanh."},
   {L"Show Timer",L"Thời gian trận",L"Hiển thị thời gian đã trôi qua trong trận."},
   {L"Show FPS",L"Số khung hình",L"Hiển thị số khung hình mỗi giây (FPS)."},
   {L"Show Mem",L"Bộ nhớ sử dụng",L"Hiển thị lượng bộ nhớ trò chơi đang sử dụng."},
   {L"Show Auto Pick",L"Nút tự nhặt đồ",L"Hiện nút bật/tắt tự động nhặt vật phẩm gần đơn vị."},
   {L"Show Drop All",L"Nút thả toàn bộ đồ",L"Hiện nút thả tất cả vật phẩm trong túi xuống đất (#dropall)."},
   {L"Show Sell All",L"Nút bán toàn bộ đồ",L"Hiện nút bán tất cả vật phẩm trong túi tại cửa hàng (#sellall)."},
   {L"Show Ins All Skills",L"Nút cộng kỹ năng",L"Hiện nút dùng điểm kỹ năng còn lại để học kỹ năng cho tướng (#allskills)."},
   {L"Text Life/Mana Commas",L"Dấu phân cách máu/năng lượng",L"Thêm dấu phân cách hàng nghìn cho số máu và năng lượng."},
   {L"Text Life/Mana No Hide",L"Giữ số máu/năng lượng",L"Giữ giá trị dài hiển thị thay vì bị ẩn khi vượt khung."},
   {L"Text Stats Commas",L"Dấu phân cách chỉ số",L"Thêm dấu phân cách hàng nghìn cho các chỉ số."},
   {L"Color Player",L"Màu thanh máu",L"Đổi màu thanh máu theo màu người chơi sở hữu đơn vị."},
   {L"VSync",L"\u0110\u1ed3ng b\u1ed9 khung h\u00ecnh",L"\u0110\u1ed3ng b\u1ed9 FPS v\u1edbi t\u1ea7n s\u1ed1 qu\u00e9t. Khi b\u1eadt, Unlock Limit FPS t\u1ea1m ng\u01b0ng."},
   {L"Unlock Limit FPS",L"B\u1ecf gi\u1edbi h\u1ea1n FPS",L"B\u1ecf kho\u1ea3ng ch\u1edd khung h\u00ecnh khi VSync t\u1eaft. T\u1eaft \u0111\u1ec3 kh\u00f4i ph\u1ee5c gi\u1edbi h\u1ea1n g\u1ed1c. FPS th\u1ef1c t\u1ebf ph\u1ee5 thu\u1ed9c m\u00e1y v\u00e0 driver."},
   {L"BarInvul",L"Ẩn thanh của đơn vị bất tử",L"Ẩn thanh máu/năng lượng khi đơn vị không thể bị gây sát thương."},
   {L"ConsoleUI",L"Ẩn bảng điều khiển",L"Ẩn giao diện điều khiển mặc định của trò chơi. Tắt để hiện lại."},
   {L"BlackBarUI",L"Ẩn dải đen",L"Ẩn dải đen của giao diện để mở rộng vùng nhìn."},
   {L"timeOfDayUI",L"Ẩn đồng hồ ngày/đêm",L"Ẩn đồng hồ thời điểm trong ngày ở phía trên màn hình."},
   {L"MouseTooltipUI",L"Chú thích theo chuột",L"Đặt chú thích của trò chơi gần vị trí chuột."},
   {L"VertexColor",L"Độ đậm màu đơn vị",L"Buộc đơn vị hiển thị không trong suốt khi áp dụng màu."}
  };
  for(const auto& e:entries)if(key==e.key){set(e.title,e.help);break;}
  if(section==L"ManaBar"||section==L"StateTag"){
   auto name=section==L"ManaBar"?L"thanh năng lượng":L"giá trị máu/năng lượng";
   if(key==L"Enable"){title=L"Hiển thị "+std::wstring(name);help=L"Bật hoặc tắt cả nhóm hiển thị này.";}
   if(key==L"EnableForHero"||key==L"EnableForNonHero"){title=L"Hiện "+std::wstring(name)+(key==L"EnableForHero"?L" cho tướng":L" cho lính");help=L"Bật hoặc tắt hiển thị cho riêng loại đơn vị này.";}}
  if(section==L"DelayClear"){bool chat=key.find(L"Chat")!=std::wstring::npos;title=chat?L"Tự xóa chat":L"Tự xóa chữ trên màn hình";help=d.kind==MenuControlKind::Choice?L"Chọn khoảng thời gian giữa các lần xóa, tính bằng giây.":chat?L"Định kỳ xóa tin nhắn chat đang hiển thị.":L"Định kỳ xóa chữ thông báo đang hiển thị trên màn hình.";}
 }
 if(section==L"Language"){title=L"Ngôn ngữ";help=L"Chọn Tiếng Việt hoặc Tiếng Anh. Lựa chọn được lưu tự động.";}
 if(section==L"MacroRecorder"){return Utf8(key==L"Enabled"?L"B\u1eadt/T\u1eaft Macro Recorder\nCho ph\u00e9p ghi, ch\u1ea1y macro v\u00e0 c\u00e1c ph\u00edm t\u1eaft macro.":key==L"ShowButton"?L"Hi\u1ec3n th\u1ecb n\u00fat Macro\nHi\u1ec7n icon v\u00e0 c\u00e1c n\u00fat thao t\u00e1c macro.":L"Ph\u00edm t\u1eaft Macro\nG\u00e1n ph\u00edm cho thao t\u00e1c n\u00e0y. Backspace x\u00f3a, Escape h\u1ee7y.");}
 if(section==L"Macro"){title=L"Macro";help=key==L"record"?L"Ghi chuột và bàn phím (Ctrl+Shift+F9). Bản ghi mới thay thế script đã lưu.":key==L"play"?L"Phát script theo số lần lặp đã lưu (Ctrl+Shift+F10). Dừng khi game mất focus.":key==L"stop"?L"Dừng ghi hoặc phát macro (Ctrl+Shift+F12).":L"Mở trình sửa script (Ctrl+Shift+F11). Thêm, sửa hoặc xóa dòng rồi kiểm tra và lưu.";}
 if(title.empty()){title=L"Tùy chọn công cụ";help=L"Bật hoặc tắt tùy chọn này.";}
 if(d.kind==MenuControlKind::Key)help+=L"\nBấm ô rồi nhấn phím để gán. Backspace xóa; Esc hủy.";
 if(d.kind==MenuControlKind::Text||d.kind==MenuControlKind::Number)help+=L"\nBấm ô để sửa. Enter lưu; Esc hủy.";
 if(!d.supported)help+=L"\nChức năng này chưa khả dụng.";
 return Utf8(title+L"\n"+help);
}
