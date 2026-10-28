#pragma once
#include "common.hpp"
#include "external_hud.hpp"
#include "clock_fps131.hpp"
namespace wht {
// Fullscreen rendering retains native frames. A caption or a client rectangle
// smaller than its monitor identifies an ordinary window without game offsets.
inline bool ExternalHudWindowed131(HWND window){
 RECT client{};MONITORINFO monitor{};monitor.cbSize=sizeof(monitor);
 if(!window||!GetClientRect(window,&client)||!GetMonitorInfoW(MonitorFromWindow(window,MONITOR_DEFAULTTONEAREST),&monitor))return false;
 return (GetWindowLongPtrW(window,GWL_STYLE)&WS_CAPTION)!=0||
  client.right<monitor.rcMonitor.right-monitor.rcMonitor.left||client.bottom<monitor.rcMonitor.bottom-monitor.rcMonitor.top;
}
inline DWORD ExternalHudFlags131(const Config& config,bool allowed,bool menuOpen,bool windowed,bool clockValid){
 if(!allowed||!config.externalHud||menuOpen||!windowed)return 0;
 return (config.showMem?HudMemory:0)|(config.showTimer&&clockValid?HudTimer:0)|
  (config.hideTimeOfDay?HudHideTime:0)|(config.workerCorePreference?HudWorkerPreference:0);
}
inline DWORD UpdateExternalHud131(ExternalHudPublisher& publisher,uintptr_t base,unsigned build,HWND window,uintptr_t ui,const Config& config,bool allowed,bool menuOpen){
 uint32_t seconds=0;const auto* profile=ClockFpsProfile(build);
 const bool valid=allowed&&config.externalHud&&config.showTimer&&profile&&SimulationSeconds131(base,*profile,seconds);
 return publisher.Publish(window,ui,ExternalHudFlags131(config,allowed,menuOpen,ExternalHudWindowed131(window),valid),seconds);
}
}
