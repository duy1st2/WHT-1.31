#pragma once
#include "goku_pick131.hpp"
#include "mth_draw_commands129.hpp"
namespace wht {
struct RouteDrawSetting131 {const wchar_t* key;bool Config::* field;};
inline constexpr RouteDrawSetting131 routeDrawSettings131[]={{L"AutoGaDraw1",&Config::autoGaDraw1},{L"AutoGaDraw2",&Config::autoGaDraw2},{L"AutoGaDraw3",&Config::autoGaDraw3},{L"AutoSoiDraw1",&Config::autoSoiDraw1},{L"AutoSoiDraw2",&Config::autoSoiDraw2},{L"AutoSoiDraw3",&Config::autoSoiDraw3}};
inline const RouteDrawSetting131* RouteDrawSettingFor131(const std::wstring& key){for(const auto& setting:routeDrawSettings131)if(key==setting.key)return &setting;return nullptr;}
inline const MthDrawCommand129* RouteDrawCommand131(const std::wstring& name){const auto* command=FindMthDrawCommand129(name);if(!command)return nullptr;for(unsigned i=0;i<command->count;++i)if(!RouteDrawSettingFor131(command->keys[i]))return nullptr;return command;}
template<class Save,class Reload> bool ApplyRouteDraw131(const MthDrawCommand129& command,const std::wstring& args,const Config& c,Save save,Reload reload){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 bool any=false;for(unsigned i=0;i<command.count;++i){const auto* setting=RouteDrawSettingFor131(command.keys[i]);if(!setting)return false;any|=c.*setting->field;}
 std::wistringstream in(args);std::wstring value;if(!ParseMthDrawValue129(in,any,value))return false;
 for(unsigned i=0;i<command.count;++i)if(!save(L"MTHKEY",command.keys[i],value))return false;reload();return true;
}
struct RouteButtonLayout131 {ToolMenuRect rect;bool shown=false;};
inline std::array<RouteButtonLayout131,6> RouteButtonLayoutFor131(const Config& c){std::array<RouteButtonLayout131,6> result{};unsigned slot=c.pickGokuDraw?1:0;for(unsigned i=0;i<6;++i){result[i].shown=c.*routeDrawSettings131[i].field;if(result[i].shown){result[i].rect={.305f+.041f*(slot%6),.142f+.028f*(slot/6),.038f,.024f};++slot;}}return result;}
struct RouteButtons131 {
 std::array<GokuIcon131,6> buttons;
 void Reset(){*this={};}
 void Update(uintptr_t base,bool allowed,const Config& c,int route,ToolMenuOverlay& menu,const NativeFrameApi* raw=nullptr){
  static const char* names[]={"Ch1","Ch2","Ch3","Wolf1","Wolf2","Wolf3"};const auto layout=RouteButtonLayoutFor131(c);
  const bool enabled=allowed&&!menu.open&&(c.mth49Enable||c.g17Enable);
  for(unsigned i=0;i<6;++i){auto& button=buttons[i];button.title=names[i];if(layout[i].shown)button.area=layout[i].rect;button.latched=route==int(i);button.Update(base,enabled&&layout[i].shown,menu,raw);}
 }
 template<class Action> bool Mouse(unsigned message,POINT point,int width,int height,bool allowed,Action action){for(unsigned i=0;i<6;++i)if(buttons[i].Mouse(message,point,width,height,allowed,[&](){action(i);}))return true;return false;}
};
}
