#pragma once
#include "tool_menu.hpp"
#include <unordered_map>
namespace wht {
struct UiCanvas129 {
    int x=0,y=0,width=0,height=0;
    POINT Project(POINT p)const{return {p.x-x,p.y-y};}
};
inline UiCanvas129 Canvas129(int width,int height){
    if(width<=0||height<=0)return {};
    return {0,0,width,height};
}
inline uintptr_t ui129Base=0;
struct UiPlacement129 {bool sized=false,absolute=false,relative=false;float width=0,x=0,y=0;int point=0,relativePoint=0,flag=1;void* owner=nullptr;};
inline std::unordered_map<void*,UiPlacement129> ui129Placements;
inline void ClearUiLayout129(){ui129Placements.clear();}
inline float ui129HorizontalScale=1.f;
inline void SetViewport129(int width,int height);
inline float UiX129(float x){return .4f+(x-.4f)*ui129HorizontalScale;}
inline void __fastcall Width129(void* object,DWORD,float width){
    auto& saved=ui129Placements[object];saved.sized=true;saved.width=width;
    using Fn=void(__thiscall*)(void*,float);
    reinterpret_cast<Fn>(ui129Base+0x473b40)(object,width*ui129HorizontalScale);
}
inline void __fastcall AbsPoint129(void* object,DWORD,int point,float x,float y,int flag){
    auto& saved=ui129Placements[object];saved.absolute=true;saved.relative=false;saved.point=point;saved.x=x;saved.y=y;saved.flag=flag;
    using Fn=void(__thiscall*)(void*,int,float,float,int);
    reinterpret_cast<Fn>(ui129Base+0x473950)(object,point,UiX129(x),y,flag);
}
inline void __fastcall RelPoint129(void* object,DWORD,int point,void* relative,int relativePoint,float x,float y,int flag){
    auto& saved=ui129Placements[object];saved.relative=true;saved.absolute=false;saved.point=point;saved.owner=relative;saved.relativePoint=relativePoint;saved.x=x;saved.y=y;saved.flag=flag;
    using Fn=void(__thiscall*)(void*,int,void*,int,float,float,int);
    reinterpret_cast<Fn>(ui129Base+0x473a10)(object,point,relative,relativePoint,x*ui129HorizontalScale,y,flag);
}
inline void SetViewport129(int width,int height){
    const int classicWidth=std::min(width,height*4/3);
    const float scale=classicWidth>0?float(width)/classicWidth:1.f;
    if(std::abs(scale-ui129HorizontalScale)<.00001f)return;
    ui129HorizontalScale=scale;
    for(const auto& entry:ui129Placements){const auto saved=entry.second;
        if(saved.sized)Width129(entry.first,0,saved.width);
        if(saved.absolute)AbsPoint129(entry.first,0,saved.point,saved.x,saved.y,saved.flag);
        else if(saved.relative)RelPoint129(entry.first,0,saved.point,saved.owner,saved.relativePoint,saved.x,saved.y,saved.flag);
    }
}
// Exact 1.29.2.9231 x86 profile. These are executable RVAs, not Game.dll RVAs.
inline void* __stdcall Allocate129(size_t size,const char* tag,int line,int flags){
    using Alloc=void*(__cdecl*)(size_t,const char*,int,int);
    return reinterpret_cast<Alloc>(ui129Base+0x40610)(size,tag,line,flags);
}
inline ExtendedFrameApi FrameApi129(uintptr_t base){
    ui129Base=base;
    nativeFrameLayout={0x128,0xc8,0xec,0x88,0x94,0xbc,0xd0,0x68,0x6c,0xac905c};
    ExtendedFrameApi api{};api.alloc=Allocate129;
#define WHT_UI129(member,rva) api.member=reinterpret_cast<decltype(api.member)>(base+rva)
    WHT_UI129(frameCtor,0x488120);WHT_UI129(backdropCtor,0x48d510);
    WHT_UI129(setBackdrop,0x488e10);WHT_UI129(stringAssign,0x68d70);
    WHT_UI129(fontCtor,0x486400);WHT_UI129(setFont,0x487470);
    WHT_UI129(setTextAlign,0x487560);WHT_UI129(setText,0x487a40);
    WHT_UI129(textureCtor,0x4864c0);WHT_UI129(setTexture,0x487c20);
    api.setWidth=reinterpret_cast<decltype(api.setWidth)>(Width129);WHT_UI129(setHeight,0x473830);
    api.setAbsPoint=reinterpret_cast<decltype(api.setAbsPoint)>(AbsPoint129);api.setRelPoint=reinterpret_cast<decltype(api.setRelPoint)>(RelPoint129);
#undef WHT_UI129
    return api;
}
using OpenArchive129=BOOL(__cdecl*)(const char*,unsigned,unsigned,HANDLE*);
// Custom assets must stay below active map archives (priorities 15/17).
// The embedded tool archive has its own higher-priority mount policy.
inline bool MountCustom129(uintptr_t base,const std::wstring& path,HANDLE& archive,OpenArchive129 open=nullptr){
 if(!base&&!open)return false;
 if(!open)open=reinterpret_cast<OpenArchive129>(base+0x39740);
 archive=nullptr;const auto encoded=Utf8(path);
 return open(encoded.c_str(),10,0,&archive)!=FALSE&&archive!=nullptr;
}
inline bool MountResources129(uintptr_t base,const std::wstring& mix,HANDLE& archive){
    using Open=BOOL(__cdecl*)(const char*,unsigned,unsigned,HANDLE*);
    // Verified against the four-argument calls mounting War3.mpq in 28BB0.
    const auto path=Utf8(mix);
    return reinterpret_cast<Open>(base+0x39740)(path.c_str(),100,2,&archive)!=FALSE;
}
}
