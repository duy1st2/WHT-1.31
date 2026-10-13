#pragma once
#include <windows.h>
#include <string>
namespace wht {
struct UpdateProgressData {volatile LONG phase=0,percent=0;};
static_assert(sizeof(UpdateProgressData)==8,"Update progress IPC layout");
inline std::string UpdateProgressText(LONG phase,LONG percent,bool english){
 std::string text;
 switch(phase){
 case 1:text=english?"New WHT update - Downloading: 0%":"Có bản cập nhật WHT mới - Đang tải: 0%";break;
 case 2:
  text=english?"Downloading WHT update":"Đang tải cập nhật WHT";
  if(percent>=0)text+=": "+std::to_string(percent>99?99:percent)+"%";
  else text+="...";
  break;
 case 3:text=english?"Verifying WHT update: 99%":"Đang kiểm tra cập nhật WHT: 99%";break;
 case 4:text=english?"WHT update downloaded: 100% - Close War3 to install":"Đã tải cập nhật WHT: 100% - Đóng War3 để cài";break;
 case 5:return std::string("|cffff5c5c")+(english?"WHT update failed - Current version retained":"Cập nhật WHT thất bại - Giữ phiên bản hiện tại")+"|r";
 default:return {};
 }
 return "|cff00ff00"+text+"|r";
}
}
