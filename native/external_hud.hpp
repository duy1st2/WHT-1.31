#pragma once
#include <windows.h>
#include <string>
#include <cstdint>
#include "worker_placement.hpp"
namespace wht {
constexpr LONG HudMagic=0x57485432,HudVersion=1;
enum HudFlags : LONG {HudTimer=1,HudMemory=2,HudHideTime=4,HudWorkerPreference=8};
struct HudPacket {DWORD pid=0,window=0,flags=0,seconds=0,epoch=0,updated=0,mainProcessor=MAXDWORD;};
struct HudShared {
    LONG magic=HudMagic,version=HudVersion;DWORD ownerPid=0;volatile LONG sequence=0;
    HudPacket packet{};
    volatile LONG ackEpoch=0,ackFlags=0,ackTick=0;
};
inline void WriteHudPacket(HudShared& shared,const HudPacket& packet){
    InterlockedIncrement(&shared.sequence);MemoryBarrier();shared.packet=packet;MemoryBarrier();InterlockedIncrement(&shared.sequence);
}
inline bool ReadHudPacket(HudShared& shared,HudPacket& packet){
    if(shared.magic!=HudMagic||shared.version!=HudVersion)return false;
    for(int i=0;i<3;++i){const LONG before=InterlockedCompareExchange(&shared.sequence,0,0);if(before&1)continue;
        MemoryBarrier();packet=shared.packet;MemoryBarrier();if(before==InterlockedCompareExchange(&shared.sequence,0,0))return true;}
    return false;
}
inline DWORD HudAcknowledged(HudShared& shared,const HudPacket& wanted,DWORD now){
    if(DWORD(InterlockedCompareExchange(&shared.ackEpoch,0,0))!=wanted.epoch||
       DWORD(now-DWORD(InterlockedCompareExchange(&shared.ackTick,0,0)))>=500)return 0;
    return DWORD(InterlockedCompareExchange(&shared.ackFlags,0,0))&wanted.flags&(HudTimer|HudMemory);
}
class ExternalHudPublisher {
    HANDLE mapping=nullptr,child=nullptr;HudShared* shared=nullptr;
    std::wstring mappingName,executable;HudPacket last{};uintptr_t owner=0;DWORD epoch=0,nextPublish=0;ULONGLONG nextLaunch=0;std::atomic<bool> wanted{false};
public:
    // Initialize before starting workers. The mapping address and names are then immutable.
    bool Initialize(const std::wstring& folder){
        if(shared)return true;
        executable=folder+L"\\WHT.ExternalHud.exe";
        mappingName=L"Local\\WHT131.ExternalHud."+std::to_wstring(GetCurrentProcessId())+L"."+std::to_wstring(GetTickCount64());
        mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(HudShared),mappingName.c_str());if(!mapping)return false;
        shared=static_cast<HudShared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(HudShared)));
        if(!shared){CloseHandle(mapping);mapping=nullptr;return false;}
        *shared=HudShared{};last.pid=GetCurrentProcessId();shared->ownerPid=last.pid;shared->packet.pid=last.pid;return true;
    }
    // Worker only: no process creation, file checks or waits on the game thread.
    void Maintain(bool enabled){
        if(!shared||!enabled||!wanted.load())return;
        if(child){if(WaitForSingleObject(child,0)==WAIT_TIMEOUT)return;CloseHandle(child);child=nullptr;}
        const auto now=GetTickCount64();if(now<nextLaunch)return;nextLaunch=now+30000;
        if(GetFileAttributesW(executable.c_str())==INVALID_FILE_ATTRIBUTES)return;
        std::wstring command=L"\""+executable+L"\" --mapping \""+mappingName+L"\"";
        STARTUPINFOW startup{};startup.cb=sizeof(startup);startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;PROCESS_INFORMATION process{};
        if(CreateProcessW(executable.c_str(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&process)){
            CloseHandle(process.hThread);child=process.hProcess;
        }
    }
    DWORD Publish(HWND window,uintptr_t root,DWORD flags,DWORD seconds){
        if(!shared)return 0;
        if(!root)flags=0;
        wanted=(flags&(HudTimer|HudMemory))!=0;
        const DWORD now=GetTickCount();const auto handle=DWORD(reinterpret_cast<uintptr_t>(window));
        if(root!=owner||flags!=last.flags||handle!=last.window){++epoch;if(!epoch)++epoch;owner=root;nextPublish=0;}
        if(!nextPublish||DWORD(now-last.updated)>=100||flags!=last.flags||handle!=last.window){
            last={GetCurrentProcessId(),handle,flags,seconds,epoch,now,gameProcessorHint.load()};WriteHudPacket(*shared,last);nextPublish=1;
        }
        if(!root||!IsWindowVisible(window)||IsIconic(window)||GetForegroundWindow()!=window)return 0;
        return HudAcknowledged(*shared,last,now);
    }
    void Hide(){wanted=false;if(shared&&last.flags){owner=0;last.flags=0;last.updated=GetTickCount();last.epoch=++epoch;WriteHudPacket(*shared,last);}}
};
}
