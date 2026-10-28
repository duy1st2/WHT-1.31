#pragma once
#include <windows.h>
#include <atomic>
#include <vector>
#include <cstdint>
#include <unordered_map>
namespace wht {
inline std::atomic<DWORD> gameProcessorHint{MAXDWORD};
inline std::atomic<bool> workerCorePreference{true};
inline void ObserveGameProcessor(){
    static thread_local ULONGLONG next=0;const auto now=GetTickCount64();
    if(now>=next){next=now+1000;gameProcessorHint=GetCurrentProcessorNumber();}
}
// A different logical processor is insufficient when it shares the same core.
inline DWORD SelectWorkerProcessor(uint64_t allowed,DWORD main,const std::vector<uint64_t>& cores){
    if(main>=64)return MAXDWORD;
    uint64_t mainCore=0;
    for(auto core:cores)if(core&(uint64_t(1)<<main)){mainCore=core;break;}
    if(!mainCore)return MAXDWORD;
    for(auto it=cores.rbegin();it!=cores.rend();++it){
        auto choices=*it&allowed;if((*it&mainCore)||!choices)continue;
        for(DWORD i=0;i<64;++i)if(choices&(uint64_t(1)<<i))return i;
    }
    return MAXDWORD;
}
inline DWORD FindWorkerProcessor(DWORD main){
    if(GetActiveProcessorGroupCount()!=1)return MAXDWORD;
    DWORD_PTR allowed=0,system=0;if(!GetProcessAffinityMask(GetCurrentProcess(),&allowed,&system))return MAXDWORD;
    using CpuSets=BOOL(WINAPI*)(PSYSTEM_CPU_SET_INFORMATION,ULONG,PULONG,HANDLE,ULONG);
    auto query=reinterpret_cast<CpuSets>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"GetSystemCpuSetInformation"));
    if(query){
        ULONG bytes=0;query(nullptr,0,&bytes,GetCurrentProcess(),0);
        if(!bytes||bytes>1024*1024)return MAXDWORD;
        std::vector<unsigned char> data(bytes);if(!query(reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(data.data()),bytes,&bytes,GetCurrentProcess(),0))return MAXDWORD;
        std::unordered_map<unsigned,uint64_t> physical;
        for(ULONG offset=0;offset<bytes;){
            auto info=reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(data.data()+offset);
            if(info->Size<sizeof(DWORD)*2||info->Size>bytes-offset)return MAXDWORD;
            if(info->Type==CpuSetInformation&&info->Size>=sizeof(SYSTEM_CPU_SET_INFORMATION)&&info->CpuSet.Group==0&&info->CpuSet.LogicalProcessorIndex<64)
                physical[info->CpuSet.CoreIndex]|=uint64_t(1)<<info->CpuSet.LogicalProcessorIndex;
            offset+=info->Size;
        }
        // Sort cores by their first logical CPU for deterministic placement.
        std::vector<uint64_t> cores;for(unsigned i=0;i<64;++i){auto it=physical.find(i);if(it!=physical.end())cores.push_back(it->second);}
        // x86 process masks can expose only their representable CPUs. Choose
        // within that conservative subset; topology still includes the main
        // thread's CPU above 31, so its SMT sibling is correctly excluded.
        return SelectWorkerProcessor(uint64_t(allowed),main,cores);
    }
    // Older Windows: do not interpret truncated x86 GROUP_AFFINITY masks.
    if(GetActiveProcessorCount(ALL_PROCESSOR_GROUPS)>sizeof(DWORD_PTR)*8)return MAXDWORD;
    DWORD bytes=0;GetLogicalProcessorInformationEx(RelationProcessorCore,nullptr,&bytes);
    if(!bytes||bytes>1024*1024)return MAXDWORD;
    std::vector<unsigned char> buffer(bytes);std::vector<uint64_t> cores;
    if(!GetLogicalProcessorInformationEx(RelationProcessorCore,reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()),&bytes))return MAXDWORD;
    for(DWORD offset=0;offset<bytes;){
        auto info=reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()+offset);
        if(info->Size<sizeof(DWORD)*2||info->Size>bytes-offset)return MAXDWORD;
        if(info->Relationship==RelationProcessorCore&&info->Processor.GroupCount==1&&info->Processor.GroupMask[0].Group==0)
            cores.push_back(info->Processor.GroupMask[0].Mask);
        offset+=info->Size;
    }
    return SelectWorkerProcessor(uint64_t(allowed),main,cores);
}
class WorkerPlacement {
    PROCESSOR_NUMBER previous{},applied{};bool owned=false;ULONGLONG next=0;
public:
    ~WorkerPlacement(){Restore();}
    void Restore(){
        PROCESSOR_NUMBER current{};
        if(owned&&GetThreadIdealProcessorEx(GetCurrentThread(),&current)&&current.Group==applied.Group&&current.Number==applied.Number)
            SetThreadIdealProcessorEx(GetCurrentThread(),&previous,nullptr);
        owned=false;
    }
    void Refresh(bool enabled,DWORD main){
        if(!enabled){Restore();next=0;return;}
        const auto now=GetTickCount64();if(now<next)return;next=now+5000;
        const DWORD target=FindWorkerProcessor(main);
        if(target==MAXDWORD){Restore();return;}
        if(owned&&target==applied.Number)return;
        PROCESSOR_NUMBER requested{};requested.Group=0;requested.Number=BYTE(target);PROCESSOR_NUMBER old{};
        if(!SetThreadIdealProcessorEx(GetCurrentThread(),&requested,&old))return;
        if(!owned)previous=old;owned=true;applied=requested;
    }
};
}
