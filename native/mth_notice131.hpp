#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
#include "mth_draw_commands129.hpp"
namespace wht {
enum class MthNoticeKind131 {None,On,Off,Acquired};
inline std::string MthNoticeText131(MthNoticeKind131 kind,bool english){
 if(english){switch(kind){case MthNoticeKind131::On:return "Auto Pick Goku: ON";case MthNoticeKind131::Off:return "Auto Pick Goku: OFF";case MthNoticeKind131::Acquired:return "Auto Pick Goku: OFF (Goku is in inventory)";default:return {};}}
 switch(kind){case MthNoticeKind131::On:return Utf8(L"T\u1ef1 nh\u1eb7t Goku: B\u1eacT");case MthNoticeKind131::Off:return Utf8(L"T\u1ef1 nh\u1eb7t Goku: T\u1eaeT");case MthNoticeKind131::Acquired:return Utf8(L"\u0110\u00e3 nh\u1eb7t Goku v\u00e0o t\u00fai - t\u1ef1 nh\u1eb7t Goku \u0111\u00e3 t\u1eaft");default:return {};}
}
struct MthNotice131 {
 MthNoticeKind131 kind=MthNoticeKind131::None;uint64_t until=0;void* frame=nullptr;void* label=nullptr;bool shown=false;std::string applied;
 void Reset(){*this={};}
 void Event(MthNoticeKind131 value,bool enabled,uint64_t now){kind=enabled?value:MthNoticeKind131::None;until=enabled?now+4000:0;}
 bool Visible(bool allowed,bool enabled,bool menu,uint64_t now){if(!allowed||!enabled||now>=until){kind=MthNoticeKind131::None;until=0;}return kind!=MthNoticeKind131::None&&!menu;}
 void Update(uintptr_t base,bool allowed,const Config& c,ToolMenuOverlay& menu,uint64_t now){
  const bool wanted=Visible(allowed,c.mthNoti,menu.open,now);if(!menu.initialized)return;
  if(wanted&&!frame)frame=menu.Frame(base,menu.independentHost,{.60f,.45f,.40f,.035f});
  if(wanted&&frame&&!label){label=menu.Font(frame,.012f,"");if(label){menu.api.setWidth(label,.40f);menu.api.setHeight(label,.035f);menu.api.setTextAlign(label,2);menu.api.setRelPoint(label,4,frame,4,0,0,1);}}
  const bool display=wanted&&frame&&label;
  if(display){const auto text=MthNoticeText131(kind,c.english);if(text!=applied){menu.api.setText(label,text.c_str());applied=text;}if(!shown)NativeFrameShow(frame);}
  else if(shown&&frame)NativeFrameHide(frame);shown=display;
 }
};
template<class Save,class Reload> bool ApplyGokuDraw131(const std::wstring& args,bool current,Save save,Reload reload){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value;if(!ParseMthDrawValue129(in,current,value))return false;
 if(!save(L"MTHKEY",L"PickGokuDraw",value))return false;reload();return true;
}
}
