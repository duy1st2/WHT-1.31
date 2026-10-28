#pragma once
template<class Check> void TestMacroToggle(Check check){
 using namespace wht;
 check(std::wstring(ToolMenuOverlay::MacroShortcutCommand(0,false))==L"#macro play","Run/Stop key starts when idle");
 check(std::wstring(ToolMenuOverlay::MacroShortcutCommand(0,true))==L"#macro stop","Run/Stop key stops playback");
 check(std::wstring(ToolMenuOverlay::MacroShortcutCommand(1,false))==L"#macro stop","legacy stop key stays compatible");
 unsigned runRows=0,stopRows=0;
 for(const auto& control:kToolMenuControls)if(std::wstring(control.section)==L"MacroRecorder"){
  if(std::wstring(control.key)==L"PlayKey")++runRows;
  if(std::wstring(control.key)==L"StopKey")++stopRows;
 }
 check(runRows==1&&stopRows==0,"Automation tab exposes one Run/Stop key");
 ToolMenuOverlay menu;menu.visible=true;menu.macroAvailable=true;menu.macroOpen=true;
 ToolMenuOverlay::SetViewport(1600,900);
 check(menu.macroActions.size()==3,"macro toolbar has three buttons");
 const auto point=[](ToolMenuRect r){return POINT{LONG((r.right-r.width/2)*1600/.8f),LONG(900-(r.bottom+r.height/2)*900/.6f)};};
 std::wstring command;auto queue=[&](const wchar_t* value){command=value;return true;};
 const auto click=[&](size_t i){auto p=point(menu.MacroActionRect(i));menu.Mouse(WM_LBUTTONDOWN,p,1600,900,queue);menu.Mouse(WM_LBUTTONUP,p,1600,900,queue);};
 toolEnglish.store(true);
 check(menu.MacroActionLabel(0)=="Run","idle toggle label");click(0);check(command==L"#macro play","idle button plays");
 menu.PlaybackVisual(true);check(menu.MacroActionLabel(0)=="Stop","playing toggle label");click(0);check(command==L"#macro stop","same button stops");
 menu.PlaybackVisual(false);check(menu.MacroActionLabel(0)=="Run","stop or natural completion restores Run");
 click(1);check(command==L"#macro record","Record remains available");click(2);check(command==L"#macro edit","Script remains available");
 check(std::wstring(ToolMenuOverlay::MacroCommand(1))==L"#macro stop","dedicated stop shortcut mapping remains compatible");
 toolEnglish.store(false);check(menu.MacroActionLabel(0)==Utf8(L"Ch\u1ea1y"),"Vietnamese Run");menu.PlaybackVisual(true);check(menu.MacroActionLabel(0)==Utf8(L"D\u1eebng"),"Vietnamese Stop");
 menu.Reset();check(!menu.macroPlaying,"UI reset clears playback state");
}
