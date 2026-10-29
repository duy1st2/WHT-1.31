#pragma once
#include "common.hpp"
#include "tool_menu.hpp"
#include "menu_fog_vertex_profile131.hpp"
#include "presentation131.hpp"
#include "slot_remap131.hpp"
namespace wht {
inline bool ControlAvailable131(const ToolMenuControl& d){
 const std::wstring section=d.section,key=d.key;
 if(MenuFogVertexControl131(section,key)||PresentationControl131(section,key))return true;
 if(section==L"Graphics"&&(key==L"Auto Reduce Effects"||key==L"timeOfDayUI"||key==L"ConsoleUI"||key==L"BlackBarUI"||key==L"MouseTooltipUI"||key==L"BarInvul"||key==L"Color Player"||key==L"Text Life/Mana Commas"||key==L"Text Life/Mana No Hide"||key==L"Text Stats Commas"||key==L"Detailed Stats Tooltip"))return true;
 if(section==L"MTHKEY"&&(key==L"Shield"||key==L"Ice"||key==L"Wild"||key==L"Frozen"||key==L"Phoenix"||key==L"ShieldDraw"||key==L"IceDraw"||key==L"WildDraw"||key==L"FrozenDraw"||key==L"PhoenixDraw"||key==L"Noti"||key==L"AutoGaDraw1"||key==L"AutoGaDraw2"||key==L"AutoGaDraw3"||key==L"AutoSoiDraw1"||key==L"AutoSoiDraw2"||key==L"AutoSoiDraw3"||key==L"ChayGa1"||key==L"ChayGa2"||key==L"ChayGa3"||key==L"ChaySoi1"||key==L"ChaySoi2"||key==L"ChaySoi3"||key==L"ChayGa1_count"||key==L"ChayGa2_count"||key==L"ChayGa3_count"||key==L"ChaySoi1_count"||key==L"ChaySoi2_count"||key==L"ChaySoi3_count"||key==L"PickGoku"||key==L"PickGokuDraw"||key==L"TimeBoss"||key==L"MTH49Enable"||key==L"G17Enable"))return true;
 if(section==L"AutoHeal"||section==L"AutoMana")return key==L"Enabled"||key==L"Percent";
 if(section==L"AutoSkills")return key==L"AutoSkills_Enable"||key==L"AutoSkills_Delay"||key==L"AutoSkills_Range"||key==L"AutoSkills_Active";
 if(section==L"Window")return key==L"Borderless"||key==L"Lock Cursor";
 if(section==L"Quick Chat")return true;
 if(section==L"DelayClear"||section==L"Language"||section==L"MacroRecorder")return true;
 if(section==L"StateTag")return true;
 if(section==L"Event")return key==L"ModeCast"||key==L"Hide Option Menu"||key==L"Keyboard Scroll Speed"||key==L"Mouse Scroll Speed"||key==L"Camera"||key==L"FogZ"||key==L"Show Mem"||key==L"ShowCooldown"||key==L"ShowRegenHP"||key==L"ShowRegenMana"||key==L"Show Hotkey In Panel"||key==L"Show Resource"||key==L"Show Timer"||key==L"Show Ping"||key==L"Show FPS"||key==L"Sound Start Game"||key==L"Quick Drop Item"||key==L"Reveal Invisibles"||key==L"Show Sell All"||key==L"Show Auto Pick"||key==L"Show Drop All"||key==L"Show Ins All Skills";
 if(section==L"HotKeyHero"&&(key==L"SetIdTarget"||key==L"SellFast"||key==L"AutoClickLeft"||key==L"AutoClickRight"||key.rfind(L"HeroF",0)==0))return true;
 if(RemapControl131(section,key))return true;
 if(section==L"HotKeyOther")return key==L"hko_menu"||key==L"hko_command"||key==L"hko_MinimizeMultiboard";
 return false;
}
}
