#pragma once
#include "tool_menu.hpp"
namespace wht {
inline std::string HudHelp131(unsigned kind){
 switch(kind){
 case 0:return toolEnglish.load()?"Drop All\nDrop carried items on the ground.":Utf8(L"Th\u1ea3 t\u1ea5t c\u1ea3\nTh\u1ea3 v\u1eadt ph\u1ea9m \u0111ang mang xu\u1ed1ng \u0111\u1ea5t.");
 case 1:return toolEnglish.load()?"All Skills\nSpend available hero skill points. Select your hero first.":Utf8(L"H\u1ecdc k\u1ef9 n\u0103ng\nD\u00f9ng \u0111i\u1ec3m k\u1ef9 n\u0103ng c\u1ee7a t\u01b0\u1edbng \u0111ang ch\u1ecdn.");
 case 2:return toolEnglish.load()?"Auto Pick\nStart or stop automatic pickup of nearby items.":Utf8(L"T\u1ef1 nh\u1eb7t \u0111\u1ed3\nB\u1eadt ho\u1eb7c d\u1eebng nh\u1eb7t v\u1eadt ph\u1ea9m \u1edf g\u1ea7n.");
 case 3:return toolEnglish.load()?"Sell All\nSell carried items at a nearby shop.":Utf8(L"B\u00e1n t\u1ea5t c\u1ea3\nB\u00e1n v\u1eadt ph\u1ea9m t\u1ea1i c\u1eeda h\u00e0ng \u1edf g\u1ea7n.");
 case 4:return toolEnglish.load()?"Goku\nStart or stop Goku pickup. Stops after the item enters inventory.":Utf8(L"Nh\u1eb7t Goku\nB\u1eadt ho\u1eb7c d\u1eebng nh\u1eb7t Goku. D\u1eebng khi v\u1eadt ph\u1ea9m v\u00e0o t\u00fai.");
 case 5:return toolEnglish.load()?"MTH crafting\nPerform this crafting action with the selected hero near the required shops.":Utf8(L"Gh\u00e9p \u0111\u1ed3 MTH\nCh\u1ecdn t\u01b0\u1edbng \u1edf g\u1ea7n c\u1eeda h\u00e0ng \u0111\u1ec3 gh\u00e9p \u0111\u1ed3.");
 case 6:return toolEnglish.load()?"Courier route\nFind the chicken or wolf, queue the configured route laps, then restore hero selection.":Utf8(L"Ch\u1ea1y tuy\u1ebfn\nT\u00ecm g\u00e0 ho\u1eb7c s\u00f3i, ch\u1ea1y s\u1ed1 v\u00f2ng \u0111\u00e3 \u0111\u1eb7t r\u1ed3i ch\u1ecdn l\u1ea1i t\u01b0\u1edbng.");
 case 7:return toolEnglish.load()?"Resources\nClick this header or press Tab to collapse or expand the resource board.":Utf8(L"T\u00e0i nguy\u00ean\nB\u1ea5m ti\u00eau \u0111\u1ec1 ho\u1eb7c Tab \u0111\u1ec3 thu g\u1ecdn hay m\u1edf b\u1ea3ng.");
 default:return {};
 }
}
// Client coordinates match the menu and HUD hit rectangles, including widescreen.
template<class Drop,class Skills,class Pick,class Sell,class Goku,class Craft,class Routes,class Resources>
void UpdateToolHelp131(HWND window,ToolMenuOverlay& menu,const Config& config,const Drop& drop,const Skills& skills,const Pick& pick,const Sell& sell,const Goku& goku,const Craft& craft,const Routes& routes,const Resources& resources){
 RECT client{};POINT point{};
 const bool foreground=window&&GetForegroundWindow()==window&&GetClientRect(window,&client)&&GetCursorPos(&point)&&ScreenToClient(window,&point);
 std::string text;
 if(foreground){
  text=menu.HelpAt(point,client.right,client.bottom,config);
  if(text.empty()&&menu.visible&&!menu.open&&menu.dropdown<0){
   auto hit=[&](const auto& icon,unsigned kind){if(text.empty()&&icon.shown&&icon.area.Hit(point,client.right,client.bottom))text=HudHelp131(kind);};
   hit(drop,0);hit(skills,1);hit(pick,2);hit(sell,3);hit(goku,4);
   for(const auto& icon:craft.buttons)hit(icon,5);
   for(const auto& icon:routes.buttons)hit(icon,6);
   if(text.empty()&&resources.visible&&ResourceBoard129::HeaderRect().Hit(point,client.right,client.bottom))text=HudHelp131(7);
  }
 }
 menu.UpdateHelp(point,client.right,client.bottom,foreground,text);
}
}
