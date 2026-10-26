#pragma once
#include "mth_route_buttons131.hpp"
#include "mth_craft131.hpp"
namespace wht {
inline constexpr RouteDrawSetting131 craftDrawSettings131[]={{L"ShieldDraw",&Config::shieldDraw},{L"IceDraw",&Config::iceDraw},{L"WildDraw",&Config::wildDraw},{L"FrozenDraw",&Config::frozenDraw},{L"PhoenixDraw",&Config::phoenixDraw}};
inline const RouteDrawSetting131* CraftDrawSetting131(const std::wstring& key){for(const auto& s:craftDrawSettings131)if(key==s.key)return &s;return nullptr;}
inline const MthDrawCommand129* CraftDrawCommand131(const std::wstring& name){auto c=FindMthDrawCommand129(name);if(!c)return nullptr;for(unsigned i=0;i<c->count;++i)if(!CraftDrawSetting131(c->keys[i]))return nullptr;return c;}
template<class Save,class Reload> bool ApplyCraftDraw131(const MthDrawCommand129& command,const std::wstring& args,const Config& c,Save save,Reload reload){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 bool any=false;for(unsigned i=0;i<command.count;++i){auto s=CraftDrawSetting131(command.keys[i]);if(!s)return false;any|=c.*s->field;}
 std::wistringstream in(args);std::wstring value;if(!ParseMthDrawValue129(in,any,value))return false;
 for(unsigned i=0;i<command.count;++i)if(!save(L"MTHKEY",command.keys[i],value))return false;reload();return true;
}
inline unsigned CraftToolbarStart131(const Config& c){unsigned n=c.pickGokuDraw?1:0;for(const auto& s:routeDrawSettings131)if(c.*s.field)++n;return n;}
struct CraftButtons131 {
 std::array<GokuIcon131,5> buttons;
 void Reset(){*this={};}
 void Update(uintptr_t base,bool allowed,const Config& c,int action,ToolMenuOverlay& menu,const NativeFrameApi* raw){
  static const char* names[]={"Shield","Ice","Wild","Frozen","Phoenix"};unsigned slot=CraftToolbarStart131(c);
  for(unsigned i=0;i<5;++i){auto& b=buttons[i];const bool show=c.*craftDrawSettings131[i].field;b.title=names[i];if(show){b.area={.305f+.041f*(slot%6),.142f+.028f*(slot/6),.038f,.024f};++slot;}
   b.latched=action==int(i);b.Update(base,allowed&&!menu.open&&(c.mth49Enable||c.g17Enable)&&show,menu,raw);}
 }
 template<class Action> bool Mouse(unsigned message,POINT point,int width,int height,bool allowed,Action action){for(unsigned i=0;i<5;++i)if(buttons[i].Mouse(message,point,width,height,allowed,[&](){action(i);}))return true;return false;}
};
}
