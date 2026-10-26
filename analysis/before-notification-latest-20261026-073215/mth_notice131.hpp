#pragma once
#include "common.hpp"
#include "notification131.hpp"
#include "tool_menu.hpp"
#include "mth_draw_commands129.hpp"
namespace wht {
enum class MthNoticeKind131 {None,On,Off,Acquired,CourierAssigned,BaseSaved,BaseRefused,CourierRequired,ReturnQueued,ReturnSent,RouteQueued,RouteSent,OrderRefused};
inline std::string MthNoticeText131(MthNoticeKind131 kind,bool english){
 switch(kind){
 case MthNoticeKind131::CourierAssigned:return english?"Courier assigned":Utf8(L"\u0110\u00e3 g\u00e1n G\u00e0/S\u00f3i");
 case MthNoticeKind131::BaseSaved:return english?"Courier base saved":Utf8(L"\u0110\u00e3 l\u01b0u \u0111i\u1ec3m c\u0103n c\u1ee9");
 case MthNoticeKind131::BaseRefused:return english?"Select a living owned unit to save the base":Utf8(L"Ch\u1ecdn unit c\u1ee7a b\u1ea1n c\u00f2n s\u1ed1ng \u0111\u1ec3 l\u01b0u c\u0103n c\u1ee9");
 case MthNoticeKind131::CourierRequired:return english?"Assign a living courier and save its base before returning":Utf8(L"Ch\u1ecdn G\u00e0/S\u00f3i: #setga. L\u01b0u c\u0103n c\u1ee9: #gabase");
 case MthNoticeKind131::ReturnQueued:return english?"Courier return requested":Utf8(L"\u0110\u00e3 nh\u1eadn l\u1ec7nh g\u1ecdi G\u00e0/S\u00f3i v\u1ec1");
 case MthNoticeKind131::ReturnSent:return english?"Return Move submitted to the game":Utf8(L"\u0110\u00e3 g\u1eedi l\u1ec7nh G\u00e0/S\u00f3i v\u1ec1 c\u0103n c\u1ee9");
 case MthNoticeKind131::RouteQueued:return english?"Chicken/Wolf route requested":Utf8(L"\u0110\u00e3 nh\u1eadn l\u1ec7nh ch\u1ea1y G\u00e0/S\u00f3i");
 case MthNoticeKind131::RouteSent:return english?"All route laps submitted to the game":Utf8(L"\u0110\u00e3 g\u1eedi \u0111\u1ee7 c\u00e1c l\u01b0\u1ee3t ch\u1ea1y G\u00e0/S\u00f3i");
 case MthNoticeKind131::OrderRefused:return english?"Courier order refused; check assigned unit and try again":Utf8(L"Kh\u00f4ng g\u1eedi \u0111\u01b0\u1ee3c l\u1ec7nh; ki\u1ec3m tra G\u00e0/S\u00f3i v\u00e0 th\u1eed l\u1ea1i");
 default:break;
 }
 if(english){switch(kind){case MthNoticeKind131::On:return "[MTH] Auto Pick Goku: On";case MthNoticeKind131::Off:return "[MTH] Auto Pick Goku: Off";case MthNoticeKind131::Acquired:return "[MTH] Da co Goku trong tui, Auto Pick Goku: Off";default:return {};}}
 switch(kind){case MthNoticeKind131::On:return Utf8(L"T\u1ef1 nh\u1eb7t Goku: B\u1eacT");case MthNoticeKind131::Off:return Utf8(L"T\u1ef1 nh\u1eb7t Goku: T\u1eaeT");case MthNoticeKind131::Acquired:return Utf8(L"\u0110\u00e3 nh\u1eb7t Goku v\u00e0o t\u00fai - t\u1ef1 nh\u1eb7t Goku \u0111\u00e3 t\u1eaft");default:return {};}
}
struct MthNotice131 {
 MthNoticeKind131 kind=MthNoticeKind131::None;uint64_t until=0;void* frame=nullptr;void* label=nullptr;bool shown=false;std::string applied;bool published=false;std::string detail;
 void Reset(){*this={};}
 void Event(MthNoticeKind131 value,bool enabled,uint64_t now){published=false;detail.clear();kind=enabled?value:MthNoticeKind131::None;until=enabled?now+4000:0;}
 void EventText(MthNoticeKind131 value,std::string text,bool enabled,uint64_t now){Event(value,enabled,now);if(enabled&&text.size()<=1024)detail=std::move(text);}
 bool Visible(bool allowed,bool enabled,bool menu,uint64_t now){if(!allowed||!enabled||now>=until){kind=MthNoticeKind131::None;until=0;}return kind!=MthNoticeKind131::None&&!menu;}
 void Update(uintptr_t base,unsigned build,uintptr_t ui,bool allowed,const Config& c,ToolMenuOverlay& menu,uint64_t now){
  if(Visible(allowed,c.mthNoti,menu.open,now)&&!published){published=ShowNotification131(base,build,ui,detail.empty()?MthNoticeText131(kind,true):detail);}
 }

};
template<class Save,class Reload> bool ApplyGokuDraw131(const std::wstring& args,bool current,Save save,Reload reload){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value;if(!ParseMthDrawValue129(in,current,value))return false;
 if(!save(L"MTHKEY",L"PickGokuDraw",value))return false;reload();return true;
}
}
