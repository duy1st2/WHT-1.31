#pragma once
#include "resource_board131.hpp"
#include <fstream>
inline void CheckResourceMouse131(){
 float point[3]{};
 check(wht::ResourceTextPoint131(.1f,.2f,.03f,.01f,0,0,.8f,.6f,100,200,point)&&std::abs(point[0]-7.4f)<.0001f&&std::abs(point[1]-23.2f)<.0001f,"glyph correction includes alignment and current viewport span");
 check(!wht::ResourceTextPoint131(.1f,.2f,.03f,.01f,.13f,.21f,.8f,.6f,100,200,point),"correct glyph position causes no native write");
 check(!wht::ResourceTextPoint131(NAN,.2f,0,0,0,0,1,1,1,1,point)&&!wht::ResourceTextPoint131(1,1,0,0,0,0,0,1,1,1,point),"invalid glyph projection refused");
 for(unsigned build:{12164u,12173u}){
#ifdef _WIN64
  const char* file=build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const char* file=build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);check(bool(input),"captured font profile required");if(!input)continue;
  std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(input)),{});const auto base=reinterpret_cast<uintptr_t>(bytes.data());const auto* p=wht::ResourceTextProfileFor131(build);check(p!=nullptr,"font projection profile available");
#ifndef _WIN64
  for(size_t n=0;n<p->count;++n){const auto& c=p->code[n];for(size_t j=0;j<c.relocCount;++j){unsigned value=0;memcpy(&value,c.bytes+c.reloc[j],4);value+=unsigned(base);memcpy(bytes.data()+c.rva+c.reloc[j],&value,4);}}
#endif
  check(wht::ResourceTextGuard131(base,*p),"complete captured font/projection functions match independent profile");bytes[p->position]^=1;check(!wht::ResourceTextGuard131(base,*p),"modified native positioning function refuses repair");
 }
 wht::ResourceBoard131 resource;resource.visible=true;
 const auto rect=wht::ResourceBoard129::HeaderRect();POINT inside{LONG((rect.right-rect.width/2)/.8f*1600),LONG((.6f-rect.bottom-rect.height/2)/.6f*900)};
 check(rect.Hit(inside,1600,900),"resource header fixture matches actual projected bounds");
 check(resource.Mouse(WM_LBUTTONDOWN,inside,1600,900,true)&&resource.expanded,"header press waits for release");resource.next=999;
 check(resource.Mouse(WM_LBUTTONUP,inside,1600,900,true)&&!resource.expanded&&!resource.next,"header release collapses and requests immediate refresh");
 check(resource.Mouse(WM_LBUTTONDBLCLK,inside,1600,900,true)&&resource.Mouse(WM_LBUTTONUP,inside,1600,900,true)&&resource.expanded,"double-click phase pairs with release as reference");
 check(resource.Mouse(WM_LBUTTONDOWN,inside,1600,900,true)&&resource.Mouse(WM_LBUTTONUP,{1500,800},1600,900,true)&&resource.expanded,"drag outside consumes release without toggle");
 check(resource.Mouse(WM_LBUTTONDOWN,inside,1600,900,true)&&resource.Mouse(WM_LBUTTONUP,inside,1600,900,false)&&resource.expanded,"menu/context change consumes owned release without toggle");
 check(!resource.Mouse(WM_LBUTTONDOWN,inside,1600,900,false)&&!resource.Mouse(WM_RBUTTONDOWN,inside,1600,900,true),"disabled header and right click pass through");
 resource.visible=false;check(!resource.Mouse(WM_LBUTTONDOWN,inside,1600,900,true),"hidden header cannot steal map click");
 resource.Reset();check(!resource.titlePressed&&!resource.visible&&resource.expanded,"teardown clears header input state");
}
