#pragma once
#include "self_cast131.hpp"
#include "time_boss131.hpp"
#include <sstream>
namespace wht {
struct Route131 {float x1,y1,x2,y2;};
inline constexpr Route131 routes131[]={{-1160,-358,-1125,-398},{-1119,-81,-1163,-125},{-882,-375,-934,-388},{1158,-397,1206,-357},{1160,-82,1212,-138},{948,-399,897,-330}};
struct PointProfile131 {unsigned build;uintptr_t entry,caller,table;};
#ifdef _WIN64
inline constexpr PointProfile131 pointProfiles131[]={{12164,0xdd7d30,0xcbbe77,0x1bfa150},{12173,0xe13ef0,0xcfbe87,0x1c28fe0}};
#else
inline constexpr PointProfile131 pointProfiles131[]={{12164,0xc5f8e0,0xb662e6,0x18206b0},{12173,0xc1aa90,0xb26016,0x17d5590}};
#endif
using PointOrder131=void(__cdecl*)(unsigned,uintptr_t,const float*,const float*,unsigned);
inline PointOrder131 PointNative131(uintptr_t base,unsigned build){
 const PointProfile131* p=nullptr;for(const auto& v:pointProfiles131)if(v.build==build)p=&v;if(!p||!base)return nullptr;
 __try{
#ifdef _WIN64
  const unsigned char prefix[]={0x48,0x81,0xec,0x88,0,0,0,0x48,0xc7,0x44,0x24,0x20,0xfe,0xff,0xff,0xff};
  const unsigned char action[]={0xc7,0x44,0x24,0x40,0x11,0,0x0a,0};
  if(memcmp((void*)(base+p->entry),prefix,sizeof(prefix))||memcmp((void*)(base+p->entry+0x16),action,sizeof(action)))return nullptr;
  const auto ref=base+p->entry+0x32;if(memcmp((void*)ref,"\x48\x8d\x05",3)||ref+7+*reinterpret_cast<int32_t*>(ref+3)!=base+p->table)return nullptr;
#else
  const unsigned char prefix[]={0x55,0x8b,0xec,0x6a,0xff};
  const unsigned char action[]={0xc7,0x45,0xcc,0x11,0,0x0a,0};
  if(memcmp((void*)(base+p->entry),prefix,sizeof(prefix))||memcmp((void*)(base+p->entry+0x2f),action,sizeof(action))||*reinterpret_cast<uintptr_t*>(base+p->entry+0x4d)!=base+p->table)return nullptr;
  if(memcmp((void*)(base+p->caller+5),"\x83\xc4\x14",3))return nullptr;
#endif
  const auto call=base+p->caller;if(*reinterpret_cast<unsigned char*>(call)!=0xe8||call+5+*reinterpret_cast<int32_t*>(call+1)!=base+p->entry)return nullptr;
  return reinterpret_cast<PointOrder131>(base+p->entry);
 }__except(1){return nullptr;}
}
inline bool RouteNonHero131(uintptr_t base,unsigned build,uintptr_t unit){
 const auto* p=BossProfileFor131(build);if(!p||!BossNative131(base,*p)||!InventoryIdentity131(unit,base,p->unit))return false;
 __try{const auto code=*reinterpret_cast<uint32_t*>(unit+bossCodeOffset131);const auto first=code>>24;return code&&!(first>='A'&&first<='Z');}__except(1){return false;}
}
inline int RouteCommand131(const std::wstring& name){const wchar_t* names[]={L"#chicken1",L"#chicken2",L"#chicken3",L"#wolf1",L"#wolf2",L"#wolf3"};for(int i=0;i<6;++i)if(name==names[i])return i;return -1;}
inline bool RouteAlias131(const std::wstring& name){return name==L"#chayga"||name==L"#chaysoi"||name==L"#caysoi";}
inline bool IsRouteCommand131(const std::wstring& name){return RouteCommand131(name)>=0||RouteAlias131(name);}
inline bool ParseRouteCommand131(const std::wstring& name,const std::wstring& args,int& route){
 route=-1;if(args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring extra;const int canonical=RouteCommand131(name);
 if(canonical>=0){if(in>>extra)return false;route=canonical;return true;}
 if(!RouteAlias131(name))return false;
 int index=1;if(args.find_first_not_of(L" \t")!=std::wstring::npos&&(!(in>>index)||in>>extra))return false;
 if(index<1||index>3)index=1;route=(name==L"#chayga"?0:3)+index-1;return true;
}
struct MthRoute131 {
 SelfCastSource131 owner;int route=-1,remaining=0;bool second=false,sent=false;uint64_t due=0,deadline=0;
 void Reset(){*this={};}
 bool Start(int index,int count,const SelfCastSource131& source){if(index<0||index>=6||count<1||count>20||!source.ui||!source.unit||!source.session||!source.id.Valid()||!std::isfinite(source.x)||!std::isfinite(source.y))return false;Reset();owner=source;route=index;remaining=count;return true;}
 template<class Read,class Send> bool TickQueued(bool enabled,bool ready,Read read,Send send){
  if(route<0)return false;if(!enabled){Reset();return true;}if(!ready)return false;
  SelfCastSource131 current;if(!read(current)||!SameSelfCastSource131(owner,current)){Reset();return true;}
  const auto r=routes131[route];const int count=remaining;Reset();send(current,r,count);return true;
 }
 template<class Read,class Send> bool Tick(uint64_t now,bool enabled,bool ready,Read read,Send send){
  if(route<0)return false;if(!enabled){Reset();return true;}if(!ready||now<due)return false;
  SelfCastSource131 current;if(!read(current)||!SameSelfCastSource131(owner,current)||!std::isfinite(current.x)||!std::isfinite(current.y)){Reset();return true;}due=now+100;
  const auto r=routes131[route];const float x=second?r.x2:r.x1,y=second?r.y2:r.y1;
  if(sent){const float dx=current.x-x,dy=current.y-y;
   if(dx*dx+dy*dy<=48.f*48.f){sent=false;if(second&&!--remaining){Reset();return true;}second=!second;}
   else if(now>=deadline){Reset();return true;}else return false;
  }
  const auto target=routes131[route];const float tx=second?target.x2:target.x1,ty=second?target.y2:target.y1;
  if(!send(current,tx,ty)){Reset();return true;}sent=true;deadline=now+30000;return false;
 }
};
}
