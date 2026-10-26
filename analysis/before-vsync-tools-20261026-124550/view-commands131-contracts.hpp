#include "casting_mode.hpp"
#pragma once
#include "view_commands131.hpp"
#include "tool_menu.hpp"
struct ViewIniTest131 {
 wchar_t file[MAX_PATH]{};
 ViewIniTest131(){wchar_t root[MAX_PATH]{};check(GetTempPathW(MAX_PATH,root)>0&&GetTempFileNameW(root,L"v13",0,file)!=0,"isolated temporary settings file");}
 ~ViewIniTest131(){if(file[0])DeleteFileW(file);}
};
inline void CheckViewCommands131(){
 wht::Config casting;auto nativeKey=[](bool inventory,int slot){return inventory?(slot==0?unsigned(VK_NUMPAD7):0u):(slot==2?unsigned('Q'):0u);};
 check(wht::FindNativeCastSlot(casting,false,'Q',nativeKey).slot<0,"normal mode preserves native targeting");casting.modeCast=1;
 check(wht::FindNativeCastSlot(casting,false,'Q',nativeKey).slot==2,"quick mode resolves original skill key");
 auto nativeItem=wht::FindNativeCastSlot(casting,false,VK_NUMPAD7,nativeKey);check(nativeItem.inventory&&nativeItem.slot==0,"quick mode resolves usable native inventory key");
 check(wht::FindNativeCastSlot(casting,false,VK_NUMPAD8,nativeKey).slot<0,"empty or unusable inventory key not claimed");
 casting.commandEnabled=true;casting.command[0].enabled=true;casting.command[0].vk='Q';check(wht::FindNativeCastSlot(casting,false,'Q',nativeKey).slot<0,"failed tool remap cannot fall through to another native skill");
 casting.commandEnabled=false;casting.inventoryEnabled=true;casting.inventory[0].enabled=true;casting.inventory[0].vk='Q';check(wht::FindNativeCastSlot(casting,false,'Q',nativeKey).slot<0,"inventory remap keeps priority over native skill");
 casting.inventoryEnabled=false;casting.shopEnabled=true;casting.shop[0].enabled=true;casting.shop[0].vk='Q';check(wht::FindNativeCastSlot(casting,true,'Q',nativeKey).slot<0,"shop remap keeps priority");
 ViewIniTest131 ini;wht::Config config=wht::LoadConfig(ini.file);unsigned writes=0,reloads=0;
 auto save=[&](const wchar_t* section,const wchar_t* key,const wchar_t* value){++writes;return wht::SaveSetting(ini.file,section,key,value);};
 auto reload=[&](){++reloads;config=wht::LoadConfig(ini.file);};
 auto run=[&](const wchar_t* name,const wchar_t* args){const auto* target=wht::ViewCommandFor131(name);check(target!=nullptr,"reference display alias exists");return wht::ApplyViewCommand131(*target,args,config,save,reload);};
 check(run(L"#timer",L"0")&&!config.showTimer&&wht::MenuRead(config,L"Event",L"Show Timer",L"1")==L"0","timer command persisted and menu agrees");
 check(run(L"#showtimer",L"")&&config.showTimer,"timer alias shares setting and bare command toggles");
 check(run(L"#res",L"1")&&config.showResource&&run(L"#resource",L"0")&&!config.showResource,"resource aliases persist same checkbox");
 check(run(L"#showhotkey",L"1")&&config.showHotkeyInPanel,"panel hotkey labels enabled through config");
 check(run(L"#statstooltip",L"0")&&!config.statsTooltip&&run(L"#statscommas",L"1")&&config.statsCommas,"graphics toggles independent");
 check(run(L"#hpmpcommas",L"1")&&config.lifeManaCommas&&run(L"#hpmpnohide",L"1")&&config.lifeManaNoHide,"portrait options persisted");
 check(run(L"#console",L"1")&&config.hideConsoleUI&&run(L"#consoleui",L"")&&!config.hideConsoleUI,"console aliases share setting");
 check(run(L"#statetag",L"1")&&config.stateTagEnable,"state values master enabled");
 check(run(L"#showmax",L"1")&&config.stateTagShowMax,"maximum values enabled independently");
 check(run(L"#hpmpvalue",L"")&&!config.stateTagEnable&&config.stateTagShowMax,"master alias toggles without losing maximum preference");
 check(wht::MenuRead(config,L"StateTag",L"Enable",L"1")==L"0"&&wht::MenuRead(config,L"StateTag",L"ShowMaxValues",L"0")==L"1","commands and menu share canonical keys");
 check(run(L"#hpmpvalue",L"1")&&config.stateTagEnable&&run(L"#showmax",L"0")&&!config.stateTagShowMax,"reenabling master preserves independent maximum control");
 check(run(L"#colorplayer",L"1")&&config.colorPlayer&&run(L"#colorp",L"")&&!config.colorPlayer,"player-color aliases share persisted toggle");
 check(run(L"#barinvul",L"1")&&config.hideInvulnerableBars&&!config.colorPlayer,"invulnerable bar preference independent of player color");
 check(wht::MenuRead(config,L"Graphics",L"Color Player",L"1")==L"0"&&wht::MenuRead(config,L"Graphics",L"BarInvul",L"0")==L"1","bar commands and menu use identical settings");
 check(run(L"#barinvul",L"0")&&!config.hideInvulnerableBars,"native bar visibility preference can be restored");
 check(run(L"#fastmenu",L"1")&&config.fastMenu&&wht::MenuRead(config,L"Event",L"Fast Menu",L"0")==L"1","fast menu command persists existing menu setting");
 check(run(L"#fastmenu",L"")&&!config.fastMenu&&run(L"#fastmenu",L"0")&&!config.fastMenu,"fast menu supports bare toggle and explicit off");
 const bool vsyncBefore=config.vsync;
 check(run(L"#unlockfps",L"1")&&config.unlockFps&&config.vsync==vsyncBefore&&wht::MenuRead(config,L"Graphics",L"Unlock Limit FPS",L"0")==L"1","FPS unlock saves preference without changing VSync");
 check(run(L"#unlockfps",L"")&&!config.unlockFps&&run(L"#unlockfps",L"0")&&!config.unlockFps,"FPS unlock supports bare toggle and explicit off");
 const auto graphicsWrites=writes,graphicsReloads=reloads;
 for(const auto* name:{L"#fastmenu",L"#unlockfps"}){
  for(const auto* args:{L"2",L"-1",L"on",L"1 extra",L"1\n"})check(!run(name,args),"graphics command rejects malformed values");
  const auto* option=wht::ViewCommandFor131(name);
  check(!wht::ApplyViewCommand131(*option,std::wstring(L"1\0x",3),config,save,reload),"graphics command rejects embedded NUL");
  check(!wht::ApplyViewCommand131(*option,L"1",config,[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload),"failed graphics save cannot reload");
 }
 check(writes==graphicsWrites&&reloads==graphicsReloads,"rejected graphics commands preserve settings");
 check(run(L"#fogclick",L"1")&&config.fogClick&&wht::MenuRead(config,L"Event",L"Fog Click",L"0")==L"1","fog click command persists exact menu key");
 check(run(L"#fc",L"")&&!config.fogClick&&run(L"#fc",L"1")&&config.fogClick&&run(L"#fogclick",L"0")&&!config.fogClick,"fog click aliases share explicit and bare-toggle state");
 check(run(L"#vertexcolor",L"1")&&config.vertexColor&&!config.fogClick&&wht::MenuRead(config,L"Graphics",L"VertexColor",L"0")==L"1","vertex color persists independently of fog observer");
 check(run(L"#vertexcolor",L"")&&!config.vertexColor,"vertex color bare command toggles saved value");
 const auto fogWrites=writes,fogReloads=reloads;
 for(const auto* name:{L"#fogclick",L"#fc",L"#vertexcolor"}){for(const auto* args:{L"2",L"-1",L"on",L"1 extra",L"1\n"})check(!run(name,args),"fog/vertex commands reject malformed settings");const auto* option=wht::ViewCommandFor131(name);
 check(!wht::ApplyViewCommand131(*option,std::wstring(L"1\0x",3),config,save,reload),"fog/vertex commands reject NUL");
 check(!wht::ApplyViewCommand131(*option,L"1",config,[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload),"fog/vertex failed saves cannot reload");}
 check(writes==fogWrites&&reloads==fogReloads,"invalid fog/vertex commands preserve saved state");
 float distance=17;
 check(wht::PresetCameraValue131(L"",distance)&&distance==2400.f,"preset camera default matches reference");
 check(wht::PresetCameraValue131(L"-100",distance)&&distance==500.f&&wht::PresetCameraValue131(L"9000",distance)&&distance==6000.f,"preset camera clamps both bounds");
 check(wht::ApplyPresetCamera131(L"3123.5",[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return save(section,key,value.c_str());},reload)&&config.camera==3123.5f&&wht::MenuRead(config,L"Event",L"Camera",L"0")==L"3123.500000","preset persists and reloads canonical camera setting");
 const unsigned presetWrites=writes,presetReloads=reloads;
 for(auto args:{L"nan",L"inf",L"1e999",L"2500 extra",L"2500\n"})check(!wht::ApplyPresetCamera131(args,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return save(section,key,value.c_str());},reload),"malformed camera preset refused");
 check(!wht::PresetCameraValue131(std::wstring(L"2500\0x",6),distance),"camera preset embedded NUL refused");
 check(!wht::ApplyPresetCamera131(L"2400",[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&writes==presetWrites&&reloads==presetReloads,"invalid preset or failed save cannot reload");
 const unsigned beforeWrites=writes,beforeReloads=reloads;const auto* target=wht::ViewCommandFor131(L"#timer");
 for(auto args:{L"2",L"on",L"0 extra",L"1 0",L"1\n0"})check(!wht::ApplyViewCommand131(*target,args,config,save,reload),"invalid input cannot edit settings");
 check(writes==beforeWrites&&reloads==beforeReloads,"malformed commands never write or reload");
 bool next=true;check(!wht::ViewToggle131(std::wstring(L"1\0x",3),false,next)&&next,"embedded NUL rejected without changing output");
 check(!wht::ApplyViewCommand131(*target,L"0",config,[](const wchar_t*,const wchar_t*,const wchar_t*){return false;},reload)&&reloads==beforeReloads,"failed persistence cannot publish new config");
 check(writes==reloads,"each successful setting change reloads exactly once");
 check(wht::ApplyCastMode131(L"1",config,save,reload)&&config.modeCast==1,"cast command enables native quick mode");
 check(wht::ApplyCastMode131(L"",config,save,reload)&&config.modeCast==0,"bare cast command switches to normal");
 check(wht::MenuRead(config,L"Event",L"ModeCast",L"1")==L"0","cast command and menu share canonical value");
 const unsigned castWrites=writes,castReloads=reloads;
 for(auto args:{L"2",L"on",L"1 extra",L"1\n0"})check(!wht::ApplyCastMode131(args,config,save,reload),"malformed cast command refused");
 check(!wht::ApplyCastMode131(L"1",config,[](const wchar_t*,const wchar_t*,const wchar_t*){return false;},reload)&&writes==castWrites&&reloads==castReloads,"failed cast setting cannot reload");
 unsigned modeControls=0;
 for(const auto& control:wht::kToolMenuControls)if(const auto* command=wht::MthModeControl131(control.section,control.key)){
  ++modeControls;check(wht::ApplyMthMode131(command,L"1",config,save,reload),"real menu mode control uses shared persistence");
  check(config.mth49Enable!=config.g17Enable,"each real menu mode selection excludes the other");
  check(wht::MenuRead(config,L"MTHKEY",L"MTH49Enable",L"0")== (config.mth49Enable?L"1":L"0")&&wht::MenuRead(config,L"MTHKEY",L"G17Enable",L"0")== (config.g17Enable?L"1":L"0"),"both menu checkbox values reflect exclusive save");
 }
 check(modeControls==2&&!wht::MthModeControl131(L"Event",L"MTH49Enable")&&!wht::MthModeControl131(L"MTHKEY",L"Noti"),"only the two MTH mode checkboxes use exclusive switching");
 auto mode=[&](const wchar_t* name,const wchar_t* args){return wht::ApplyMthMode131(name,args,config,save,reload);};
 check(mode(L"#g17",L"on")&&config.g17Enable&&!config.mth49Enable,"enabling G17 disables MTH49");
 check(mode(L"#mth49",L"true")&&config.mth49Enable&&!config.g17Enable,"enabling MTH49 disables G17");
 check(mode(L"#mth49",L"")&&!config.mth49Enable&&!config.g17Enable,"bare mode toggles off without enabling other mode");
 check(mode(L"#g17",L"1")&&mode(L"#mth49",L"0")&&config.g17Enable,"disabling inactive mode preserves active mode");
 check(run(L"#mthnoti",L"0")&&!config.mthNoti&&config.g17Enable,"notifications independent of active mode");
 const unsigned modeWrites=writes,modeReloads=reloads;
 for(auto args:{L"2",L"on extra",L"1\n0"})check(!mode(L"#g17",args),"malformed mode never edits config");
 check(writes==modeWrites&&reloads==modeReloads,"malformed mode leaves config untouched");
 unsigned failedSaves=0;check(!wht::ApplyMthMode131(L"#mth49",L"1",config,[&](const wchar_t*,const wchar_t*,const wchar_t*){return ++failedSaves<2;},reload)&&failedSaves==2&&reloads==modeReloads,"second write failure cannot publish a reload or success");
 check(wht::SaveSetting(ini.file,L"Event",L"Camera",L"2750"),"external config edit saved");
 const unsigned externalWrites=writes,externalReloads=reloads;
 check(wht::ReloadConfigCommand131(L" \t",reload)&&config.camera==2750.f&&writes==externalWrites&&reloads==externalReloads+1,"explicit reload reads external edits without writing config");
 for(auto args:{L"1",L"extra",L"\n",L"\r"})check(!wht::ReloadConfigCommand131(args,reload),"reload takes no arguments or line injection");
 check(!wht::ReloadConfigCommand131(std::wstring(L"\0",1),reload)&&reloads==externalReloads+1,"NUL and malformed reloads cannot invoke callback");
 check(!wht::ViewCommandFor131(L"#autoskills"),"gameplay actions are not routed through display settings");
 check(!wht::ViewCommandFor131(L"#menu"),"configured tool menu command retains existing behavior");
}
