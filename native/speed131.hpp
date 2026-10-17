#pragma once
#include <windows.h>
#include <MinHook.h>
#include "speed_clock129.hpp"
namespace wht {
using SpeedQpc131=BOOL(WINAPI*)(LARGE_INTEGER*);
using SpeedTick131=DWORD(WINAPI*)();
SpeedQpc131 originalSpeedQpc131=nullptr;
SpeedTick131 originalSpeedTime131=nullptr,originalSpeedTick131=nullptr;
std::mutex speedClockMutex131;
ScaledClock129 speedQpcClock131;
ScaledTick129 speedTimeClock131,speedTickClock131;
bool speedClocksReady131=false;
float speedMultiplier131=1.f;
bool ApplySpeedMultiplier131(float speed){
 if(!speedClocksReady131||!std::isfinite(speed)||speed<1||speed>70)return false;
 std::lock_guard<std::mutex> lock(speedClockMutex131);
 LARGE_INTEGER now{};if(!originalSpeedQpc131(&now))return false;
 speedQpcClock131.Set(now.QuadPart,speed);
 speedTimeClock131.Set(originalSpeedTime131(),speed);speedTickClock131.Set(originalSpeedTick131(),speed);speedMultiplier131=speed;return true;
}
BOOL WINAPI SpeedQpcHook131(LARGE_INTEGER* result){
 if(!originalSpeedQpc131||!originalSpeedQpc131(result))return FALSE;
 std::lock_guard<std::mutex> lock(speedClockMutex131);
 result->QuadPart=static_cast<LONGLONG>(speedQpcClock131.Read(result->QuadPart));return TRUE;
}
DWORD WINAPI SpeedTimeHook131(){const auto now=originalSpeedTime131();std::lock_guard<std::mutex> lock(speedClockMutex131);return speedTimeClock131.Read(now);}
DWORD WINAPI SpeedTickHook131(){const auto now=originalSpeedTick131();std::lock_guard<std::mutex> lock(speedClockMutex131);return speedTickClock131.Read(now);}
void InstallSpeedClocks131(){
 const auto status=MH_Initialize();if(status!=MH_OK&&status!=MH_ERROR_ALREADY_INITIALIZED)return;
 auto kernel=GetModuleHandleW(L"kernel32.dll");auto multimedia=GetModuleHandleW(L"winmm.dll");
 if(!kernel||!multimedia)return;
 void* addresses[]={reinterpret_cast<void*>(GetProcAddress(kernel,"QueryPerformanceCounter")),reinterpret_cast<void*>(GetProcAddress(multimedia,"timeGetTime")),reinterpret_cast<void*>(GetProcAddress(kernel,"GetTickCount"))};
 void* hooks[]={reinterpret_cast<void*>(SpeedQpcHook131),reinterpret_cast<void*>(SpeedTimeHook131),reinterpret_cast<void*>(SpeedTickHook131)};
 void** originals[]={reinterpret_cast<void**>(&originalSpeedQpc131),reinterpret_cast<void**>(&originalSpeedTime131),reinterpret_cast<void**>(&originalSpeedTick131)};
 unsigned created=0,enabled=0;
 for(;created<3;++created)if(!addresses[created]||MH_CreateHook(addresses[created],hooks[created],originals[created])!=MH_OK)break;
 if(created==3)for(;enabled<3;++enabled)if(MH_EnableHook(addresses[enabled])!=MH_OK)break;
 speedClocksReady131=enabled==3;
 if(!speedClocksReady131){for(unsigned i=0;i<enabled;++i)MH_DisableHook(addresses[i]);for(unsigned i=0;i<created;++i)MH_RemoveHook(addresses[i]);originalSpeedQpc131=nullptr;originalSpeedTime131=originalSpeedTick131=nullptr;}
}

inline bool SetSpeed131(const std::wstring& args){float speed=1;if(!ParseSpeedMultiplier129(args,speed))return false;if(!speedClocksReady131)InstallSpeedClocks131();return ApplySpeedMultiplier131(speed);}
inline void ResetSpeed131(){if(speedClocksReady131&&speedMultiplier131!=1.f)ApplySpeedMultiplier131(1.f);}
}
