#pragma once
#include "common.hpp"
#include "view_commands131.hpp"
namespace wht {
inline bool SameWindowRect131(const RECT& a,const RECT& b){return a.left==b.left&&a.top==b.top&&a.right==b.right&&a.bottom==b.bottom;}
inline bool WindowClipWanted131(bool lock,bool foreground,bool iconic,bool menu){return lock&&foreground&&!iconic&&!menu;}
inline bool WriteWindowStyle131(HWND window,LONG_PTR style){SetLastError(0);const auto old=SetWindowLongPtrW(window,GWL_STYLE,style);return old!=0||GetLastError()==0;}
struct WindowServices131 {
 HWND owner=nullptr;bool borderless=false,clipped=false;LONG_PTR style=0;WINDOWPLACEMENT placement{sizeof(WINDOWPLACEMENT)};RECT writtenClip{};
 void ReleaseClip(bool foregroundRelease=false){if(!clipped&&!foregroundRelease)return;RECT current{};if(GetClipCursor(&current)&&(foregroundRelease||SameWindowRect131(current,writtenClip)))ClipCursor(nullptr);clipped=false;}
 bool Borderless(bool wanted){
  if(wanted==borderless)return true;if(!owner||!IsWindow(owner)){borderless=false;return false;}
  if(wanted){MONITORINFO info{sizeof(info)};if(!GetMonitorInfoW(MonitorFromWindow(owner,MONITOR_DEFAULTTONEAREST),&info)||!GetWindowPlacement(owner,&placement))return false;
   SetLastError(0);style=GetWindowLongPtrW(owner,GWL_STYLE);if(!style&&GetLastError())return false;
   if(!IsWindowVisible(owner))placement.showCmd=SW_HIDE;
   if(!WriteWindowStyle131(owner,style&~WS_OVERLAPPEDWINDOW))return false;
   if(!SetWindowPos(owner,nullptr,info.rcMonitor.left,info.rcMonitor.top,info.rcMonitor.right-info.rcMonitor.left,info.rcMonitor.bottom-info.rcMonitor.top,SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED)){WriteWindowStyle131(owner,style);SetWindowPlacement(owner,&placement);return false;}borderless=true;
  }else {if(!WriteWindowStyle131(owner,style))return false;if(!SetWindowPlacement(owner,&placement))return false;if(!SetWindowPos(owner,nullptr,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED))return false;borderless=false;}
  return true;
 }
 void Restore(){ReleaseClip();if(borderless)Borderless(false);owner=nullptr;borderless=false;}
 bool Apply(HWND window,bool full,bool lock,bool menu){
  if(!window||!IsWindow(window)){Restore();return false;}if(owner!=window){Restore();owner=window;}
  const bool ok=Borderless(full);RECT client{};const bool active=WindowClipWanted131(lock,GetForegroundWindow()==window,IsIconic(window)!=FALSE,menu)&&GetClientRect(window,&client)&&client.right>client.left&&client.bottom>client.top;
  if(active){POINT a{client.left,client.top},b{client.right,client.bottom};if(!ClientToScreen(window,&a)||!ClientToScreen(window,&b)){ReleaseClip();return false;}RECT rect{a.x,a.y,b.x,b.y},current{};
   if(!GetClipCursor(&current)||!SameWindowRect131(current,rect)){if(!ClipCursor(&rect))return false;}writtenClip=rect;clipped=true;
  }else ReleaseClip(GetForegroundWindow()==window);return ok;
 }
};
inline bool ParseWindowSize131(const std::wstring& args,int& width,int& height){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;std::wistringstream in(args);int w=0,h=0;std::wstring extra;if(!(in>>w>>h)||in>>extra||w<320||h<240||w>16384||h>16384)return false;width=w;height=h;return true;
}
inline bool ResizeWindow131(HWND window,int width,int height){return window&&IsWindow(window)&&!IsIconic(window)&&width>=320&&width<=16384&&height>=240&&height<=16384&&SetWindowPos(window,nullptr,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED)!=FALSE;}
inline std::string WindowSizeText131(HWND window){RECT outer{},client{};if(!window||!GetWindowRect(window,&outer)||!GetClientRect(window,&client))return {};return "Window Size: "+std::to_string(outer.right-outer.left)+"x"+std::to_string(outer.bottom-outer.top)+" | Game: "+std::to_string(client.right-client.left)+"x"+std::to_string(client.bottom-client.top);}
template<class Save,class Reload> bool ApplyWindowToggle131(const std::wstring& name,const std::wstring& args,const Config& config,Save save,Reload reload){
 if(name!=L"#full"&&name!=L"#lock")return false;bool value=false;if(!ViewToggle131(args,name==L"#full"?config.borderless:config.lockCursor,value))return false;
 if(!save(L"Window",name==L"#full"?L"Borderless":L"Lock Cursor",value?L"1":L"0"))return false;reload();return true;
}
}
