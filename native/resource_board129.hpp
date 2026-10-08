#pragma once
#include "tool_menu.hpp"
#include "resource129.hpp"
#include "player_colors129.hpp"
namespace wht {
struct ResourceBoard129 {
 void* header=nullptr;void* title=nullptr;void* arrow=nullptr;
 struct Row {void* frame=nullptr;std::array<void*,4> text{};};
 std::array<Row,24> rows{};
 // Reserve the left hero portrait strip, including its selection border.
 static constexpr float Left=.034f,Top=.578f,Width=.225f,HeaderHeight=.01885f,RowHeight=.018f,Step=.0165f;
 static ToolMenuRect HeaderRect(){return {Left+Width,Top-HeaderHeight,Width,HeaderHeight};}
 static ToolMenuRect RowRect(size_t i){return {Left+Width,Top-HeaderHeight-float(i)*Step-RowHeight,Width,RowHeight};}
 void Hide(){if(header)NativeFrameHide(header);}
 bool Present(ToolMenuOverlay& menu,uintptr_t base,const ResourceTable& table,bool expanded){
  if(!menu.initialized||!table.ready){Hide();return false;}
  auto& api=menu.api;
  if(!header){
   header=menu.Frame(base,menu.independentHost,HeaderRect());if(!header)return false;
   NativeSetFrameBackdropEx(api,header,"WHTUI_GreyBackground.tga","UI\\Widgets\\EscMenu\\Human\\editbox-border.blp",.01f,.004f);
   title=menu.TextIn(header,HeaderRect(),"|cffffff00Show Resource (Tab)|r",true,.0114075f);if(!title)return false;
   arrow=menu.Texture(header,"UI\\Widgets\\Glues\\GlueScreen-Pulldown-Arrow.blp",.008f,.008f);
   if(arrow)api.setRelPoint(arrow,5,header,5,-.004f,0,1);
  }
  for(size_t i=0;i<rows.size();++i){auto& row=rows[i];
   if(!expanded||i>=table.rows.size()){if(row.frame)NativeFrameHide(row.frame);continue;}
   const auto rect=RowRect(i);
   if(!row.frame){
    row.frame=menu.Frame(base,header,rect);if(!row.frame)return false;
    NativeSetFrameBackdropEx(api,row.frame,"WHTUI_GreyBackground.tga","UI\\Widgets\\ToolTips\\Human\\human-tooltip-border.blp",.0075f,.003f);
    constexpr float offsets[]={.0075f,.09f,.135f,.18f};
    constexpr float widths[]={.08f,.042f,.042f,.042f};
    for(size_t c=0;c<4;++c){
     row.text[c]=menu.Font(row.frame,.012f,"");if(!row.text[c])return false;
     api.setWidth(row.text[c],widths[c]);api.setHeight(row.text[c],RowHeight);
     api.setTextAlign(row.text[c],1);
     api.setRelPoint(row.text[c],3,row.frame,3,offsets[c],0,1);
    }
   }
   const auto& value=table.rows[i];
   const std::string gray=value.left?"|cff7d7d7d":"";
   auto name=Utf8(value.name);std::replace(name.begin(),name.end(),'|',' ');
   const std::array<std::string,4> text={
    "|cffffff00"+gray+std::to_string(value.slot+1)+"  "+PlayerColorTag129(value.color,value.left)+name+"|r",
    "|cffffff00"+gray+Utf8(ResourceNumber(value.gold))+"|r",
    "|cff20c000"+gray+Utf8(ResourceNumber(value.lumber))+"|r",
    "|cfff7a58b"+gray+Utf8(ResourceNumber(value.food))+(value.cap>0?"/"+Utf8(ResourceNumber(value.cap)):"")+"|r"};
   for(size_t c=0;c<4;++c)api.setText(row.text[c],text[c].c_str());
   NativeFrameShow(row.frame);
  }
  NativeFrameShow(header);return true;
 }
};
}
