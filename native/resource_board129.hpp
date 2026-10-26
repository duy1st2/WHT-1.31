#pragma once
#include "tool_menu.hpp"
#include "resource129.hpp"
#include "player_colors129.hpp"
namespace wht {
struct ResourceBoard129 {
 void* header=nullptr;void* title=nullptr;void* arrow=nullptr;bool headerShown=false,arrowKnown=false,lastExpanded=false;
 struct Row {void* frame=nullptr;std::array<void*,4> text{},applied{};std::array<std::string,4> cached{};bool shown=false;};
 std::array<Row,24> rows{};
 // Reserve the left hero portrait strip, including its selection border.
 static constexpr float Left=.034f,Top=.578f,Width=.225f,HeaderHeight=.01885f,RowHeight=.018f,Step=.018f;
 static constexpr float CellOffset(size_t column){return column==0?.004f:column==1?.116f:column==2?.153f:.19f;}
 static ToolMenuRect HeaderRect(){return {Left+Width,Top-HeaderHeight,Width,HeaderHeight};}
 static ToolMenuRect RowRect(size_t i){return {Left+Width,Top-HeaderHeight-float(i)*Step-RowHeight,Width,RowHeight};}
 void Hide(){for(auto& row:rows)if(row.frame&&row.shown){NativeFrameHide(row.frame);row.shown=false;}if(header&&headerShown){NativeFrameHide(header);headerShown=false;}}
 bool Present(ToolMenuOverlay& menu,uintptr_t base,const ResourceTable& table,bool expanded){
  if(!menu.initialized||!table.ready){Hide();return false;}
  auto& api=menu.api;
  if(!header){
   header=menu.Frame(base,menu.independentHost,HeaderRect());if(!header)return false;
   NativeSetFrameBackdropEx(api,header,"WHTUI_GreyBackground.tga","UI\\Widgets\\EscMenu\\Human\\editbox-border.blp",.01f,.004f);
   title=menu.TextIn(header,HeaderRect(),"|cffffff00Show Resource (Tab)|r",true,.0114075f);if(!title)return false;
   arrow=menu.Texture(header,"UI\\Widgets\\Glues\\Gluescreen-Scrollbar-UpArrow.blp",.008f,.008f);
   if(arrow)api.setRelPoint(arrow,5,header,5,-.004f,0,1);
  }
  if(arrow&&(!arrowKnown||lastExpanded!=expanded)){api.setTexture(arrow,expanded?"UI\\Widgets\\Glues\\Gluescreen-Scrollbar-UpArrow.blp":"UI\\Widgets\\Glues\\Gluescreen-Scrollbar-DownArrow.blp",0);arrowKnown=true;lastExpanded=expanded;}
  if(!headerShown){NativeFrameShow(header);headerShown=true;}
  for(size_t i=0;i<rows.size();++i){auto& row=rows[i];
   if(!expanded||i>=table.rows.size()){if(row.frame&&row.shown){NativeFrameHide(row.frame);row.shown=false;}continue;}
   const auto rect=RowRect(i);
   if(!row.frame){
    row.frame=menu.Frame(base,header,rect);if(!row.frame)return false;
    NativeSetFrameBackdropEx(api,row.frame,"WHTUI_GreyBackground.tga","UI\\Widgets\\ToolTips\\Human\\human-tooltip-border.blp",.0075f,.003f);
    for(size_t c=0;c<4;++c){
     row.text[c]=menu.Font(row.frame,.0105f,"");if(!row.text[c])return false;
     api.setTextAlign(row.text[c],1);
     api.setRelPoint(row.text[c],3,row.frame,3,CellOffset(c),0,1);
    }
   }
   const auto& value=table.rows[i];
   const std::string gray=value.left?"|cff7d7d7d":"";
   auto label=value.name;if(label.size()>18)label=label.substr(0,17)+L"?";
   auto name=Utf8(label);std::replace(name.begin(),name.end(),'|',' ');
   const std::array<std::string,4> text={
    "|cffffff00"+gray+std::to_string(value.slot+1)+"  "+PlayerColorTag129(value.color,value.left)+name+"|r",
    "|cffffff00"+gray+Utf8(ResourceNumber(value.gold))+"|r",
    "|cff20c000"+gray+Utf8(ResourceNumber(value.lumber))+"|r",
    "|cfff7a58b"+gray+Utf8(ResourceNumber(value.food))+(value.cap>0?"/"+Utf8(ResourceNumber(value.cap)):"")+"|r"};
   for(size_t c=0;c<4;++c)if(row.applied[c]!=row.text[c]||row.cached[c]!=text[c]){api.setText(row.text[c],text[c].c_str());api.setRelPoint(row.text[c],3,row.frame,3,CellOffset(c),0,1);row.applied[c]=row.text[c];row.cached[c]=text[c];}
   if(!row.shown){NativeFrameShow(row.frame);row.shown=true;for(size_t c=0;c<4;++c)api.setRelPoint(row.text[c],3,row.frame,3,CellOffset(c),0,1);}
  }
  return true;
 }
};
}
