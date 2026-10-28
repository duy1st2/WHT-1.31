#include "../native/external_hud131.hpp"
#include "../native/local_files131.hpp"
#include "casting_mode.hpp"
#pragma once
#include "worker_placement.hpp"
#include <thread>
#include <exception>
#include "menu_capabilities131.hpp"
#include "menu_search131.hpp"
#include "hotkey_diagnostics131.hpp"
#include "quick_chat_settings131.hpp"
#include "view_commands131.hpp"
#include "tool_menu.hpp"
struct ViewIniTest131 {
 wchar_t file[MAX_PATH]{};
 ViewIniTest131(){wchar_t root[MAX_PATH]{};check(GetTempPathW(MAX_PATH,root)>0&&GetTempFileNameW(root,L"v13",0,file)!=0,"isolated temporary settings file");}
 ~ViewIniTest131(){if(file[0])DeleteFileW(file);}
};
inline void CheckViewCommands131(){
 {wht::Config cfg;cfg.externalHud=true;cfg.showMem=true;cfg.showTimer=true;
 auto flags=wht::ExternalHudFlags131(cfg,true,false,true,true);
 check((flags&(wht::HudMemory|wht::HudTimer))==(wht::HudMemory|wht::HudTimer),"external HUD publishes requested displays");
 check(!wht::ExternalHudFlags131(cfg,true,true,true,true),"F8 menu stays above external HUD");
 check(!wht::ExternalHudFlags131(cfg,false,false,true,true)&&!wht::ExternalHudFlags131(cfg,true,false,false,true),"unauthorized and fullscreen retain native HUD");
 check(!(wht::ExternalHudFlags131(cfg,true,false,true,false)&wht::HudTimer),"invalid native clock cannot suppress timer");
 cfg.externalHud=false;check(!wht::ExternalHudFlags131(cfg,true,false,true,true),"external HUD disabled by default policy");
 }

 {bool stored=false;int writes=0,nativeWrites=0,reloads=0;
 auto save=[&](const wchar_t*,const wchar_t*,const wchar_t* value){++writes;stored=std::wstring(value)==L"1";return true;};
 auto writer=[&](bool value){++nativeWrites;return value;};
 auto reload=[&](){++reloads;};
 check(wht::LocalFilesCommand131(L"#alf")&&wht::LocalFilesCommand131(L"#allowlocal")&&wht::LocalFilesCommand131(L"#localfiles")&&!wht::LocalFilesCommand131(L"#other"),"reference local file aliases");
 check(wht::ApplyLocalFiles131(L"",false,save,writer,reload)&&stored&&reloads==1&&nativeWrites==1,"local files toggle saves and publishes once");
 check(!wht::ApplyLocalFiles131(L"0",true,save,writer,reload)&&stored&&reloads==1,"failed registry write restores prior INI setting");
 const auto before=writes;for(auto args:{L"2",L"1 extra",L"1\n"})check(!wht::ApplyLocalFiles131(args,false,save,writer,reload)&&writes==before,"invalid local files input has no side effects");
 check(!wht::ApplyLocalFiles131(L"1",false,[](const wchar_t*,const wchar_t*,const wchar_t*){return false;},writer,reload)&&nativeWrites==2,"failed persistence never invokes registry writer");
 }

 wht::Config casting;auto nativeKey=[](bool inventory,int slot){return inventory?(slot==0?unsigned(VK_NUMPAD7):0u):(slot==2?unsigned('Q'):0u);};
 check(wht::FindNativeCastSlot(casting,false,'Q',nativeKey).slot<0,"normal mode preserves native targeting");casting.modeCast=1;
 check(wht::FindNativeCastSlot(casting,false,'Q',nativeKey).slot==2,"quick mode resolves original skill key");
 auto nativeItem=wht::FindNativeCastSlot(casting,false,VK_NUMPAD7,nativeKey);check(nativeItem.inventory&&nativeItem.slot==0,"quick mode resolves usable native inventory key");
 check(wht::FindNativeCastSlot(casting,false,VK_NUMPAD8,nativeKey).slot<0,"empty or unusable inventory key not claimed");
 casting.commandEnabled=true;casting.command[0].enabled=true;casting.command[0].vk='Q';check(wht::FindNativeCastSlot(casting,false,'Q',nativeKey).slot<0,"failed tool remap cannot fall through to another native skill");
 casting.commandEnabled=false;casting.inventoryEnabled=true;casting.inventory[0].enabled=true;casting.inventory[0].vk='Q';check(wht::FindNativeCastSlot(casting,false,'Q',nativeKey).slot<0,"inventory remap keeps priority over native skill");
 casting.inventoryEnabled=false;casting.shopEnabled=true;casting.shop[0].enabled=true;casting.shop[0].vk='Q';check(wht::FindNativeCastSlot(casting,true,'Q',nativeKey).slot<0,"shop remap keeps priority");

 {wht::Config keys;keys.commandKey=0;keys.multiboardKey=0;keys.mth49Enable=false;keys.g17Enable=false;
 std::string report;
 check(wht::HotkeyCheckText131(keys,L"",report)&&report.find("no configured conflicts")!=std::string::npos,"empty hotkeys have no synthetic zero-key collisions");
 keys.quickChatEnabled=true;keys.quickChat[0]={VK_F2,"first"};keys.quickChat[1]={VK_F2,"second"};
 auto conflicts=wht::HotkeyConflicts131(keys);check(conflicts.size()==1&&conflicts[0].key==VK_F2,"duplicate enabled chat shortcuts detected once");
 keys.quickChatEnabled=false;check(wht::HotkeyConflicts131(keys).empty(),"disabled quick chat cannot cause active conflict report");
 keys.commandEnabled=keys.shopEnabled=true;keys.command[0].enabled=keys.shop[0].enabled=true;keys.command[0].vk=keys.shop[0].vk='Q';
 check(wht::HotkeyConflicts131(keys).empty(),"normal and shop grids do not conflict across mutually exclusive contexts");
 keys.inventoryEnabled=true;keys.inventory[0].enabled=true;keys.inventory[0].vk='Q';
 check(wht::HotkeyConflicts131(keys).size()==2,"inventory overlaps each enabled grid context separately");
 keys.quickChatEnabled=true;keys.quickChat[2]={VK_F2,"third"};keys.menuKey=VK_F2;keys.heroes[0]=VK_F2;
 check(wht::HotkeyCheckText131(keys,L"1",report)&&report.find("next page")!=std::string::npos&&report.find("F2")!=std::string::npos,"conflict report includes named keys and pagination");
 check(wht::HotkeyCheckText131(keys,L"2",report),"additional conflict page is readable");
 const auto savedReport=report;for(auto args:{L"0",L"9999",L"-1",L"1 extra",L"1\n"})check(!wht::HotkeyCheckText131(keys,args,report)&&report==savedReport,"invalid diagnostics cannot publish partial result");
 check(!wht::HotkeyCheckText131(keys,std::wstring(L"1\0x",3),report),"diagnostic command rejects NUL");
 check(keys.inventory[0].vk=='Q'&&keys.quickChat[0].text=="first"&&keys.menuKey==VK_F2,"diagnostics leave bindings and text unchanged");
 check(wht::DiagnosticKey131(VK_NUMPAD7)=="NUMPAD7"&&wht::DiagnosticKey131(VK_TAB)=="TAB","diagnostic key names are stable");}

 {std::wstring query;check(wht::MenuSearchQuery131(L"  H\u1ed2I MANA  ",query)&&query==L"hoi mana","Vietnamese search removes accents and folds case");
 const auto all=[](const wht::ToolMenuControl&){return true;};
 check(wht::MenuSearchFold131(L"Hi\u1ec3n th\u1ecb t\u1ed1c \u0111\u1ed9 h\u1ed3i Mana/s Show mana regeneration Event ShowRegenMana").find(L"hoi mana")!=std::wstring::npos,"normalization retries when Windows size estimate is insufficient");
 auto hits=wht::FindMenuControls131(query,all);check(!hits.empty(),"unaccented Vietnamese finds regeneration option");
 check(wht::MenuSearchQuery131(L"ping",query)&&!wht::FindMenuControls131(query,all).empty(),"menu search finds new ping checkbox");
 check(wht::MenuSearchQuery131(L"delay",query),"English/INI keyword query accepted");
 hits=wht::FindMenuControls131(query,all);check(hits.size()>1,"multiple matching menu options can be navigated");
 wht::ToolMenuOverlay menu;wht::MenuSearch131 search;std::string notice;
 menu.editing=3;menu.dropdown=4;menu.editBuffer=L"old";
 check(search.Run(L"delay",menu,all,notice)&&menu.open&&menu.page==hits[0].page&&menu.editing==-1&&menu.dropdown==-1&&menu.editBuffer.empty(),"search opens matching tab and cancels unsaved editor/dropdown");
 check(search.Run(L"DELAY",menu,all,notice)&&search.cursor==1&&menu.page==hits[1].page,"same normalized search advances result");
 for(size_t i=1;i<hits.size();++i)check(search.Run(L"delay",menu,all,notice),"all results can be cycled");
 check(search.cursor==0,"result cycling wraps to first match");
 const auto priorPage=menu.page;const auto priorNotice=notice;
 for(auto bad:{L"",L" ",L"ping\n"})check(!search.Run(bad,menu,all,notice)&&menu.page==priorPage&&notice==priorNotice,"invalid query cannot change UI or publish partial result");
 check(!search.Run(std::wstring(L"ping\0x",6),menu,all,notice),"search rejects NUL");
 check(search.Run(L"unlikelymissingoption123",menu,all,notice)&&menu.page==priorPage&&notice.find("no matching")!=std::string::npos,"missing result leaves current menu tab unchanged");
 check(wht::FindMenuControls131(L"ping",[](const wht::ToolMenuControl&){return false;}).empty(),"unavailable native controls are not offered by search");
 search.Reset();check(search.query.empty()&&search.cursor==0,"match teardown resets search state");}

 {size_t options=0;bool castFound=false;wht::Config defaults;
 for(size_t i=0;i<wht::kToolMenuControls.size();++i){const auto& d=wht::kToolMenuControls[i];if(d.kind==wht::MenuControlKind::Label)continue;
  ++options;check(d.supported&&wht::ControlAvailable131(d),"every current interactive menu option has an implemented capability on both architectures");
  if(!wcscmp(d.section,L"Event")&&!wcscmp(d.key,L"ModeCast")){castFound=true;check(d.kind==wht::MenuControlKind::Choice&&wht::ToolMenuOverlay::Enabled(defaults,i),"casting mode dropdown is available without parent prerequisites");
   check(wht::ValidMenuValue(d,L"0")&&wht::ValidMenuValue(d,L"1")&&!wht::ValidMenuValue(d,L"2"),"casting mode exposes only Normal and Quick Cast");}
 }
 check(options>200&&castFound,"complete menu inventory includes casting mode");
 wht::ToolMenuControl foreign{};foreign.section=L"Event";foreign.key=L"UnknownNativeFeature";foreign.supported=true;
 check(!wht::ControlAvailable131(foreign),"unknown native features remain locked");
 auto profile=std::make_shared<wht::Config::Profile>();defaults.profile=profile;
 size_t gated=0;for(size_t i=0;i<wht::kToolMenuControls.size();++i){const auto& d=wht::kToolMenuControls[i];if(std::wstring(d.section)==L"Quick Chat"&&std::wstring(d.key)==L"QuickChatHotkey1"){
  (*profile)[L"quick chat"][L"enable"]=L"0";check(!wht::ToolMenuOverlay::Enabled(defaults,i),"chat shortcut stays disabled while its master is off");
  (*profile)[L"quick chat"][L"enable"]=L"1";check(wht::ToolMenuOverlay::Enabled(defaults,i),"chat master unlocks shortcut without changing native capabilities");++gated;}}
 check(gated==1,"parent dependency audit covers actual chat control");}

 {const std::vector<uint64_t> cores{3,12,48};
 check(wht::SelectWorkerProcessor(63,0,cores)==4,"ported worker placement selects another physical core");
 check(wht::SelectWorkerProcessor(3,0,cores)==MAXDWORD,"SMT sibling alone is not a separate physical core");
 check(wht::SelectWorkerProcessor(15,0,cores)==2,"worker preference respects available process CPUs");
 check(wht::SelectWorkerProcessor(63,MAXDWORD,cores)==MAXDWORD,"unknown UI processor does not guess a destination");
 PROCESSOR_NUMBER before{},after{};check(GetThreadIdealProcessorEx(GetCurrentThread(),&before)!=FALSE,"capture test main-thread hint");
 const DWORD main=GetCurrentProcessorNumber();std::exception_ptr failure;
 std::thread background([&](){try{PROCESSOR_NUMBER original{},observed{};check(GetThreadIdealProcessorEx(GetCurrentThread(),&original)!=FALSE,"capture worker hint");
  const auto target=wht::FindWorkerProcessor(main);wht::WorkerPlacement placement;placement.Refresh(true,main);
  if(target!=MAXDWORD)check(GetThreadIdealProcessorEx(GetCurrentThread(),&observed)&&observed.Group==0&&observed.Number==target,"native Windows ideal-processor preference applies to worker");
  placement.Refresh(false,main);check(GetThreadIdealProcessorEx(GetCurrentThread(),&observed)&&observed.Group==original.Group&&observed.Number==original.Number,"turning off restores worker's previous hint");
 }catch(...){failure=std::current_exception();}});background.join();if(failure)std::rethrow_exception(failure);
 check(GetThreadIdealProcessorEx(GetCurrentThread(),&after)&&before.Group==after.Group&&before.Number==after.Number,"worker preference leaves main-thread hint unchanged");}
 ViewIniTest131 ini;wht::Config config=wht::LoadConfig(ini.file);unsigned writes=0,reloads=0;
 auto save=[&](const wchar_t* section,const wchar_t* key,const wchar_t* value){++writes;return wht::SaveSetting(ini.file,section,key,value);};
 auto reload=[&](){++reloads;config=wht::LoadConfig(ini.file);};
 auto run=[&](const wchar_t* name,const wchar_t* args){const auto* target=wht::ViewCommandFor131(name);check(target!=nullptr,"reference display alias exists");return wht::ApplyViewCommand131(*target,args,config,save,reload);};

 check(run(L"#showfps",L"0")&&run(L"#showping",L"0")&&run(L"#hideclock",L"0"),"monitoring options initialize independently");
 check(run(L"#showfps",L"1")&&config.showFps&&!config.showPing&&!config.hideTimeOfDay,"FPS command does not enable ping or hide day/night");
 check(run(L"#showping",L"1")&&config.showPing&&config.showFps,"ping command preserves FPS");
 const bool timerBeforeMonitor=config.showTimer;
 check(run(L"#hideclock",L"1")&&config.hideTimeOfDay&&config.showTimer==timerBeforeMonitor&&config.showPing,"day/night visibility is independent of match timer and ping");
 for(auto name:{L"#showfps",L"#showping",L"#hideclock"}){
  const auto* option=wht::ViewCommandFor131(name);
  check(wht::MenuRead(config,option->section,option->key,L"0")==L"1","monitoring command and checkbox share persisted key");
  check(run(name,L"")&&!(config.*option->field),"bare monitoring command toggles off");
  check(run(name,L"1")&&(config.*option->field)&&run(name,L"0")&&!(config.*option->field),"monitoring command supports explicit on/off");
  const auto monitorWrites=writes,monitorReloads=reloads;
  for(auto args:{L"2",L"-1",L"on",L"1 extra",L"1\n"})check(!run(name,args),"invalid monitoring commands preserve settings");
  check(!wht::ApplyViewCommand131(*option,std::wstring(L"1\0x",3),config,save,reload),"monitoring commands reject embedded NUL");
  check(!wht::ApplyViewCommand131(*option,L"1",config,[](const wchar_t*,const wchar_t*,const wchar_t*){return false;},reload),"failed monitoring save does not publish state");
  check(writes==monitorWrites&&reloads==monitorReloads,"rejected monitoring command never writes or reloads");
 }
 check(!wht::ViewCommandFor131(L"#ping"),"one-shot ping remains separate from persistent display toggle");

 check(run(L"#autocleartext",L"0")&&run(L"#autoclearchat",L"0"),"auto clear channels initialize separately");
 auto delay=[&](const wchar_t* name,const std::wstring& args){return wht::ApplyClearDelay131(name,args,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return save(section,key,value.c_str());},reload);};
 check(delay(L"#cleartextdelay",L"1")&&config.clearTextSeconds==1&&!config.clearTextEnabled,"editing text interval does not enable auto clear");
 check(delay(L"#clearchatdelay",L"17")&&config.clearChatSeconds==17&&config.clearTextSeconds==1&&!config.clearChatEnabled,"chat and text intervals stay independent");
 check(wht::MenuRead(config,L"DelayClear",L"Delay Clear Chat Screen",L"0")==L"17","chat delay shares real menu seconds key");
 check(run(L"#autocleartext",L"1")&&config.clearTextEnabled&&!config.clearChatEnabled,"text auto clear does not enable chat clear");
 check(run(L"#autoclearchat",L"")&&config.clearChatEnabled&&config.clearTextEnabled,"chat auto clear supports independent bare toggle");
 check(run(L"#autocleartext",L"0")&&!config.clearTextEnabled&&config.clearTextSeconds==1&&config.clearChatEnabled,"disabling text clear preserves both intervals and other channel");
 const auto clearWrites=writes,clearReloads=reloads;
 for(auto name:{L"#cleartextdelay",L"#clearchatdelay"}){
  for(auto args:{L"",L"0",L"18",L"9999999999999",L"-1",L"+1",L"1.5",L"1 extra",L"1\n"})check(!delay(name,args),"invalid auto clear delay is refused without clamping");
  check(!delay(name,std::wstring(L"1\0x",3)),"auto clear delay rejects embedded NUL");
  check(!wht::ApplyClearDelay131(name,L"3",[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload),"failed interval save does not reload");
 }
 for(auto name:{L"#autocleartext",L"#autoclearchat"})for(auto args:{L"2",L"1 extra",L"1\n"})check(!run(name,args),"invalid auto clear toggle is refused");
 check(writes==clearWrites&&reloads==clearReloads,"invalid auto clear input cannot edit or publish config");
 check(!delay(L"#clear",L"3")&&!wht::ViewCommandFor131(L"#clear"),"manual clear remains separate from automatic settings");

 auto qedit=[&](const wchar_t* name,const std::wstring& args){return wht::ApplyQuickChatSetting131(name,args,[&](const wchar_t* section,const wchar_t* key,const std::wstring& value){return save(section,key,value.c_str());},reload);};
 check(run(L"#quickchat",L"0")&&!config.quickChatEnabled,"quick chat command disables existing master checkbox");
 const auto existingKey=config.quickChat[0].vk;
 check(qedit(L"#qchat",L"1 defend mid")&&config.quickChat[0].text=="defend mid"&&config.quickChat[0].vk==existingKey&&!config.quickChatEnabled,"editing quick chat preserves master and binding");
 check(qedit(L"#qchat",L"9 \"quoted text\"  ")&&config.quickChat[8].text=="\"quoted text\"  ","message quotes and trailing spaces round-trip through real INI");
 check(run(L"#quickchat",L"1")&&config.quickChatEnabled&&wht::MenuRead(config,L"Quick Chat",L"Enable",L"0")==L"1","quick chat toggle and menu agree");
 check(qedit(L"#qchatclear",L"1")&&config.quickChat[0].text.empty()&&config.quickChat[8].text=="\"quoted text\"  "&&config.quickChat[0].vk==existingKey&&config.quickChatEnabled,"clear affects only requested text slot");
 check(run(L"#quickchat",L"")&&!config.quickChatEnabled,"bare quick chat command toggles");
 const auto qWrites=writes,qReloads=reloads;
 for(auto args:{L"0 text",L"10 text",L"-1 text",L"1",L"1 ",L"1 hello\nworld"})check(!qedit(L"#qchat",args),"invalid quick chat editing is refused");
 check(!qedit(L"#qchat",L"1 "+std::wstring(129,L'a'))&&!qedit(L"#qchat",std::wstring(L"1 a\0b",5)),"oversize and NUL messages cannot save");
 for(auto args:{L"0",L"10",L"1 extra",L"1\n"})check(!qedit(L"#qchatclear",args),"quick chat clear refuses bad slot or extra arguments");
 check(!wht::ApplyQuickChatSetting131(L"#qchat",L"2 test",[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&writes==qWrites&&reloads==qReloads,"failed or invalid quick chat edits never reload settings");
 wht::QuickChatSetting131 unicodeQuick;
 check(wht::ParseQuickChatSetting131(L"#qchat",L"2 Ti\u1ebfng Vi\u1ec7t",unicodeQuick)&&unicodeQuick.text==L"Ti\u1ebfng Vi\u1ec7t","quick chat parser preserves Vietnamese text");
 check(!wht::ParseQuickChatSetting131(L"#qchat",L"2 "+std::wstring(65,L'\u00e9'),unicodeQuick),"quick chat limit counts UTF-8 bytes");

 const auto chat9Text=config.quickChat[8].text;const auto chatMaster=config.quickChatEnabled;
 check(qedit(L"#qchatkey",L"1 F2")&&config.quickChat[0].vk==VK_F2&&wht::MenuRead(config,L"Quick Chat",L"QuickChatHotkey1",L"-1")==L"769","F2 binding persists the engine code, not the Windows virtual key");
 check(qedit(L"#qchatkey",L"9 a")&&config.quickChat[8].vk=='A'&&config.quickChat[8].text==chat9Text&&config.quickChatEnabled==chatMaster&&config.quickChat[0].vk==VK_F2,"binding changes preserve text, master and other slots");
 check(qedit(L"#qchatkey",L"2 NUMPAD7")&&config.quickChat[1].vk==VK_NUMPAD7,"numpad binding maps through existing engine translation");
 check(qedit(L"#qchatkey",L"1 none")&&!config.quickChat[0].vk&&config.quickChat[8].vk=='A',"unbinding clears only requested shortcut");
 int engineKey=0;for(unsigned i=1;i<=12;++i)check(wht::QuickChatKey131(L"F"+std::to_wstring(i),engineKey)&&wht::EngineKeyToVirtualKey(engineKey)==VK_F1+i-1,"all supported function keys round-trip");
 check(wht::QuickChatKey131(L"SPACE",engineKey)&&wht::EngineKeyToVirtualKey(engineKey)==VK_SPACE&&wht::QuickChatKey131(L"TAB",engineKey)&&wht::EngineKeyToVirtualKey(engineKey)==VK_TAB,"named key mapping");
 const auto keyWrites=writes,keyReloads=reloads;
 for(auto args:{L"0 F2",L"10 F2",L"1",L"1 F13",L"1 ESC",L"1 ENTER",L"1 CTRL",L"1 NUMPAD10",L"1 F2 extra",L"1 F2\n"})check(!qedit(L"#qchatkey",args),"unsupported keys and malformed bindings cannot save");
 check(!qedit(L"#qchatkey",std::wstring(L"1 F2\0x",6))&&!wht::ApplyQuickChatSetting131(L"#qchatkey",L"1 F2",[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&writes==keyWrites&&reloads==keyReloads,"NUL or failed key save never publishes settings");

 check(config.workerCorePreference,"worker preference default matches both reference versions");
 check(!config.externalHud,"external HUD defaults off");
 check(run(L"#externalhud",L"1")&&config.externalHud&&wht::MenuRead(config,L"Performance",L"External HUD",L"0")==L"1","external HUD saves reference INI setting");
 check(run(L"#externalhud",L"")&&!config.externalHud&&run(L"#externalhud",L"0")&&!config.externalHud,"external HUD toggle and disable persist");
 const auto hudWrites=writes,hudReloads=reloads;
 for(auto args:{L"2",L"1 extra",L"1\n"})check(!run(L"#externalhud",args),"invalid external HUD settings rejected");
 check(!wht::ApplyViewCommand131(*wht::ViewCommandFor131(L"#externalhud"),L"1",config,[](const wchar_t*,const wchar_t*,const wchar_t*){return false;},reload)&&writes==hudWrites&&reloads==hudReloads&&!config.externalHud,"failed external HUD save never enables helper");
 check(run(L"#workercores",L"0")&&!config.workerCorePreference&&wht::MenuRead(config,L"Performance",L"Worker Core Preference",L"1")==L"0","worker cores persists exact reference configuration key");
 check(run(L"#workercores",L"")&&config.workerCorePreference&&run(L"#workercores",L"1")&&config.workerCorePreference,"bare and explicit worker-core enable semantics match reference");
 const auto workerWrites=writes,workerReloads=reloads;
 for(auto args:{L"2",L"on",L"-1",L"1 extra",L"1\n"})check(!run(L"#workercores",args),"worker-core malformed command refused");
 check(!wht::ApplyViewCommand131(*wht::ViewCommandFor131(L"#workercores"),std::wstring(L"1\0x",3),config,save,reload),"worker cores refuses NUL");
 check(!wht::ApplyViewCommand131(*wht::ViewCommandFor131(L"#workercores"),L"0",config,[](const wchar_t*,const wchar_t*,const wchar_t*){return false;},reload)&&writes==workerWrites&&reloads==workerReloads&&config.workerCorePreference,"failed worker preference save does not publish config");
 check(run(L"#hn",L"0")&&!config.showNotiKickBan&&run(L"#hn",L"")&&config.showNotiKickBan,"moderation notification command persists and toggles the canonical preference");
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
 const bool savedUnlock=config.unlockFps;
 check(run(L"#vsync",L"1")&&config.vsync&&config.unlockFps==savedUnlock&&wht::MenuRead(config,L"Graphics",L"VSync",L"0")==L"1","VSync command uses case-insensitive native/menu key and preserves unlock preference");
 check(run(L"#vsync",L"")&&!config.vsync&&run(L"#vsync",L"0")&&!config.vsync,"VSync bare toggle and explicit disable");
 const auto vsyncWrites=writes,vsyncReloads=reloads;for(auto args:{L"2",L"on",L"1 extra",L"1\n"})check(!run(L"#vsync",args),"VSync malformed inputs refused");
 check(!wht::ApplyViewCommand131(*wht::ViewCommandFor131(L"#vsync"),L"1",config,[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&writes==vsyncWrites&&reloads==vsyncReloads,"VSync failed/invalid saves do not reload");
 unsigned toggles=0;for(auto name:{L"#tools",L"#tool",L"#menu"})check(wht::ToolMenuCommand131(name,L" \t",[&](){++toggles;}),"tool menu aliases dispatch once");
 for(auto args:{L"1",L"extra",L"\n"})check(!wht::ToolMenuCommand131(L"#tools",args,[&](){++toggles;}),"menu alias rejects arguments");
 check(!wht::ToolMenuCommand131(L"#tools",std::wstring(L"\0",1),[&](){++toggles;})&&toggles==3,"menu alias rejects NUL without toggling");
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
