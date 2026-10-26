#pragma once
#include "common.hpp"
#include "notification131.hpp"
#include "tool_menu.hpp"
#include "mth_draw_commands129.hpp"
namespace wht {
enum class MthNoticeKind131 {None,On,Off,Acquired,CourierAssigned,BaseSaved,BaseRefused,CourierRequired,ReturnQueued,ReturnSent,RouteQueued,RouteSent,OrderRefused};
inline std::string MthNoticeText131(MthNoticeKind131 kind,bool english){
 switch(kind){
 case MthNoticeKind131::CourierAssigned:return english?"Courier assigned":Utf8(L"?? g?n G?/S?i");
 case MthNoticeKind131::BaseSaved:return english?"Courier base saved":Utf8(L"?? l?u ?i?m c?n c?");
 case MthNoticeKind131::BaseRefused:return english?"Select a living owned unit to save the base":Utf8(L"Ch?n unit c?a b?n c?n s?ng ?? l?u c?n c?");
 case MthNoticeKind131::CourierRequired:return english?"Assign a living courier and save its base before returning":Utf8(L"Ch?n G?/S?i: #setga. L?u c?n c?: #gabase");
 case MthNoticeKind131::ReturnQueued:return english?"Courier return requested":Utf8(L"?? nh?n l?nh g?i G?/S?i v?");
 case MthNoticeKind131::ReturnSent:return english?"Return Move submitted to the game":Utf8(L"?? g?i l?nh G?/S?i v? c?n c?");
 case MthNoticeKind131::RouteQueued:return english?"Chicken/Wolf route requested":Utf8(L"?? nh?n l?nh ch?y G?/S?i");
 case MthNoticeKind131::RouteSent:return english?"All route laps submitted to the game":Utf8(L"?? g?i ?? c?c l??t ch?y G?/S?i");
 case MthNoticeKind131::OrderRefused:return english?"Courier order refused; check assigned unit and try again":Utf8(L"Kh?ng g?i ???c l?nh; ki?m tra G?/S?i v? th? l?i");
 default:break;
 }
 if(english){switch(kind){case MthNoticeKind131::On:return "Auto Pick Goku: ON";case MthNoticeKind131::Off:return "Auto Pick Goku: OFF";case MthNoticeKind131::Acquired:return "Auto Pick Goku: OFF (Goku is in inventory)";default:return {};}}
 switch(kind){case MthNoticeKind131::On:return Utf8(L"T\u1ef1 nh\u1eb7t Goku: B\u1eacT");case MthNoticeKind131::Off:return Utf8(L"T\u1ef1 nh\u1eb7t Goku: T\u1eaeT");case MthNoticeKind131::Acquired:return Utf8(L"\u0110\u00e3 nh\u1eb7t Goku v\u00e0o t\u00fai - t\u1ef1 nh\u1eb7t Goku \u0111\u00e3 t\u1eaft");default:return {};}
}
struct MthNotice131 {
 MthNoticeKind131 kind=MthNoticeKind131::None;uint64_t until=0;void* frame=nullptr;void* label=nullptr;bool shown=false;std::string applied;bool published=false;std::string detail;
 void Reset(){*this={};}
 void Event(MthNoticeKind131 value,bool enabled,uint64_t now){published=false;detail.clear();kind=enabled?value:MthNoticeKind131::None;until=enabled?now+4000:0;}
 void EventText(MthNoticeKind131 value,std::string text,bool enabled,uint64_t now){Event(value,enabled,now);if(enabled&&text.size()<=1024)detail=std::move(text);}
 bool Visible(bool allowed,bool enabled,bool menu,uint64_t now){if(!allowed||!enabled||now>=until){kind=MthNoticeKind131::None;until=0;}return kind!=MthNoticeKind131::None&&!menu;}
 void Update(uintptr_t base,unsigned build,uintptr_t ui,bool allowed,const Config& c,ToolMenuOverlay& menu,uint64_t now){
  if(Visible(allowed,c.mthNoti,menu.open,now)&&!published){published=true;ShowNotification131(base,build,ui,detail.empty()?MthNoticeText131(kind,true):detail);}
 }

};
template<class Save,class Reload> bool ApplyGokuDraw131(const std::wstring& args,bool current,Save save,Reload reload){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value;if(!ParseMthDrawValue129(in,current,value))return false;
 if(!save(L"MTHKEY",L"PickGokuDraw",value))return false;reload();return true;
}
}
