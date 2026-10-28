#include "../native/external_hud.hpp"
#include "../native/hud_layout129.hpp"
#include <stdexcept>
#include <cstdio>
void Check(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
int main(){try{
    using namespace wht;
    for(float height:{600.f,900.f,1080.f,1440.f}){
        const float timerBottom=height*(.6f-TimerCenterY129+TimerTextHeight129*.5f)/.6f;
        const float memoryTop=height*(.6f-MemoryCenterY129-MemoryTextHeight129*.5f)/.6f;
        const float memoryBottom=height*(.6f-MemoryCenterY129+MemoryTextHeight129*.5f)/.6f;
        const float fpsTop=height*(.6f-FpsCenterY129-FpsTextHeight129*.5f)/.6f;
        Check(timerBottom<memoryTop&&memoryBottom<fpsTop,"compact timer/memory/FPS text rows never overlap at supported heights");
        Check((TimerCenterY129-MemoryCenterY129)*900/.6f<27&&(MemoryCenterY129-FpsCenterY129)*900/.6f<27,"900px HUD rows stay compact");
    }
    const std::vector<uint64_t> cores{3,12,48};
    Check(SelectWorkerProcessor(63,0,cores)==4,"worker uses a different physical core");
    Check(SelectWorkerProcessor(3,0,cores)==MAXDWORD,"SMT sibling alone is not an alternate core");
    Check(SelectWorkerProcessor(15,0,cores)==2,"worker respects process affinity");
    Check(SelectWorkerProcessor(63,MAXDWORD,cores)==MAXDWORD,"unknown game processor does not guess a core");
    Check(SelectWorkerProcessor(63,2,cores)==4,"main on another core still excludes siblings");
    {
        const DWORD main=GetCurrentProcessorNumber(),target=FindWorkerProcessor(main);
        PROCESSOR_NUMBER original{},observed{};Check(GetThreadIdealProcessorEx(GetCurrentThread(),&original)!=FALSE,"read previous ideal processor without changing it");
        WorkerPlacement placement;placement.Refresh(true,main);
        if(target!=MAXDWORD)Check(GetThreadIdealProcessorEx(GetCurrentThread(),&observed)&&observed.Group==0&&observed.Number==target,"real worker ideal-processor hint applied");
        placement.Refresh(false,main);
        Check(GetThreadIdealProcessorEx(GetCurrentThread(),&observed)&&observed.Group==original.Group&&observed.Number==original.Number,"disabling restores previous worker hint");
    }
    HudShared shared{};HudPacket packet{},read{};packet.pid=GetCurrentProcessId();packet.window=123;packet.flags=HudTimer|HudMemory;packet.seconds=1234;packet.epoch=7;packet.updated=100;
    WriteHudPacket(shared,packet);Check(ReadHudPacket(shared,read)&&read.pid==packet.pid&&read.seconds==1234,"versioned numeric IPC snapshot");
    InterlockedIncrement(&shared.sequence);Check(!ReadHudPacket(shared,read),"incomplete publication is never consumed");InterlockedIncrement(&shared.sequence);
    shared.ackEpoch=7;shared.ackTick=100;shared.ackFlags=HudTimer|HudMemory;
    Check(HudAcknowledged(shared,packet,599)==(HudTimer|HudMemory),"healthy host acknowledges rendered HUD");
    Check(!HudAcknowledged(shared,packet,600),"stale host restores native HUD within 500ms");
    packet.epoch=8;Check(!HudAcknowledged(shared,packet,101),"old match/config acknowledgement is not reused");packet.epoch=7;
    packet.flags=HudTimer;Check(HudAcknowledged(shared,packet,101)==HudTimer,"disabled displays never inherit host acknowledgement");
    shared.ackTick=LONG(0xfffffff0u);Check(HudAcknowledged(shared,packet,10)==HudTimer,"heartbeat timeout handles DWORD wrap");
    shared.version=2;Check(!ReadHudPacket(shared,read),"incompatible IPC rejected");
    ExternalHudPublisher publisher;Check(!publisher.Publish(nullptr,1,HudTimer,1),"missing host retains native HUD");
    std::puts("PASS physical core selection, numeric IPC, incomplete/version rejection, fallback, flags and rollover");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
