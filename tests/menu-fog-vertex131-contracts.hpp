#pragma once
#include "menu_fog_vertex131.hpp"
inline unsigned vertexObserved131=0;inline bool vertexChanges131=false,vertexThrows131=false;inline float glueElapsedTest131=0,glueTimeTest131=0;inline uintptr_t glueContextTest131=0;inline unsigned fogAddsTest131=0,fogRemovesTest131=0;
#ifdef _WIN64
inline uintptr_t VertexStub131(uintptr_t unit){
#else
inline uintptr_t __fastcall VertexStub131(uintptr_t unit,void*){
#endif
 vertexObserved131=*reinterpret_cast<unsigned char*>(unit+0x50);if(vertexThrows131)RaiseException(0xe0310311,0,0,nullptr);if(vertexChanges131)*reinterpret_cast<unsigned char*>(unit+0x50)=77;return 123;
}
#ifdef _WIN64
inline uintptr_t ModelGetStub131(uintptr_t frame){
#else
inline uintptr_t __fastcall ModelGetStub131(uintptr_t frame,void*){
#endif
 return *reinterpret_cast<uintptr_t*>(frame+sizeof(uintptr_t));
}
#ifdef _WIN64
inline uintptr_t GlueStub131(uintptr_t,float elapsed,float time,uintptr_t context){
#else
inline uintptr_t __fastcall GlueStub131(uintptr_t,void*,float elapsed,float time,uintptr_t context){
#endif
 glueElapsedTest131=elapsed;glueTimeTest131=time;glueContextTest131=context;return 321;
}
inline void __cdecl FogAddStub131(unsigned char event,void* callback,void* context,unsigned flags){check(event==0x16&&callback==reinterpret_cast<void*>(wht::FogClickCapture131)&&!context&&!flags,"observer registration preserves native event/context ABI");++fogAddsTest131;}
inline void __cdecl FogRemoveStub131(unsigned char event,void* callback,void* context){check(event==0x16&&callback==reinterpret_cast<void*>(wht::FogClickCapture131)&&!context,"observer removal identifies only the tool callback");++fogRemovesTest131;}
inline uintptr_t fogPointOwner131=0;
#ifdef _WIN64
inline uintptr_t FogPointStub131(uintptr_t property,float* values){
#else
inline uintptr_t __fastcall FogPointStub131(uintptr_t property,void*,float* values){
#endif
 fogPointOwner131=property;values[0]=10;values[1]=20;return property;
}
inline bool VertexExceptionTest131(uintptr_t unit){__try{
#ifdef _WIN64
 wht::VertexColorCall131(unit);
#else
 wht::VertexColorCall131(unit,nullptr);
#endif
 return false;}__except(1){return true;}}
inline void CheckMenuFogVertex131(){
 using namespace wht;
#ifdef _WIN64
 const unsigned durationPids[]={6688,29928};
#else
 const unsigned durationPids[]={14740,23884};
#endif
 for(unsigned n=0;n<2;++n){std::ifstream file("../analysis/memory-"+std::to_string(durationPids[n])+".bin",std::ios::binary);check(bool(file),"transition native fixture available");std::vector<char> image((std::istreambuf_iterator<char>(file)),{});const auto* profile=FastMenuTransitionFor131(n?12173:12164);const auto base=reinterpret_cast<uintptr_t>(image.data());
#ifndef _WIN64
  for(size_t i=0;i<profile->code.relocCount;++i){auto at=profile->entry+profile->code.reloc[i];unsigned rva=0;memcpy(&rva,profile->code.bytes+profile->code.reloc[i],4);unsigned value=unsigned(base)+rva;memcpy(image.data()+at,&value,4);}
#endif
  check(FastMenuTransitionGuard131(base,*profile),"complete transition body and original callback verified for each build");image[profile->entry+20]^=1;check(!FastMenuTransitionGuard131(base,*profile),"changed transition implementation refused");
 }

 unsigned transitionCalls=0;
 auto transition=[&](unsigned kind,uintptr_t sequence,unsigned a,unsigned b,uintptr_t callback,unsigned context){++transitionCalls;check(kind==4&&sequence==uintptr_t(-2)&&a==12&&b==13&&callback==uintptr_t(-3)&&context==99,"transition ABI preserves sequence/callback pointers and all native arguments");return 2.5f;};
 check(RunFastMenuTransition131(true,transition,4,uintptr_t(-2),12,13,uintptr_t(-3),99)==.001f&&transitionCalls==1,"Fast Menu calls native exactly once then caps transition wait like WHT");
 check(RunFastMenuTransition131(false,transition,4,uintptr_t(-2),12,13,uintptr_t(-3),99)==2.5f&&transitionCalls==2,"disabled Fast Menu returns exact original duration");
 check(FastMenuDuration131(0,true)==0&&FastMenuDuration131(-1,true)==-1&&FastMenuDuration131(.0005f,true)==.0005f&&std::isnan(FastMenuDuration131(NAN,true)),"missing/invalid/already-short native durations retain reference behavior");

 auto memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));check(memory!=nullptr,"isolated data-slot fixture allocated");if(!memory)return;const uintptr_t at=reinterpret_cast<uintptr_t>(memory);*reinterpret_cast<uintptr_t*>(at)=11;DWORD old=0;VirtualProtect(memory,4096,PAGE_READONLY,&old);
 NativeDataSlot131 slot;check(slot.Install(at,11,22)&&*reinterpret_cast<uintptr_t*>(at)==22,"owned read-only data slot atomically replaced");MEMORY_BASIC_INFORMATION info{};VirtualQuery(memory,&info,sizeof(info));check(info.Protect==PAGE_READONLY,"data slot page protection restored");check(slot.Install(at,11,22)&&!slot.Install(at,11,33),"idempotent slot install and foreign replacement refusal");check(slot.Restore()&&*reinterpret_cast<uintptr_t*>(at)==11,"native data slot restored");NativeDataSlot131 wrong;check(!wrong.Install(at,12,33)&&*reinterpret_cast<uintptr_t*>(at)==11,"foreign original remains untouched");VirtualFree(memory,0,MEM_RELEASE);
 alignas(uintptr_t) unsigned char unit[0x100]{};*reinterpret_cast<uintptr_t*>(unit)=0x1234;unit[0x4f]=56;unit[0x50]=12;unit[0x51]=78;vertexOriginal131=reinterpret_cast<uintptr_t>(VertexStub131);vertexTable131=0x1234;vertexAlpha131=0x50;vertexEnabled131=true;
#ifdef _WIN64
 const auto result=VertexColorCall131(reinterpret_cast<uintptr_t>(unit));
#else
 const auto result=VertexColorCall131(reinterpret_cast<uintptr_t>(unit),nullptr);
#endif
 check(result==123&&vertexObserved131==255&&unit[0x50]==12&&unit[0x4f]==56&&unit[0x51]==78,"native color callback sees opaque alpha, exact original return/RGB/alpha preserved");
 vertexThrows131=true;check(VertexExceptionTest131(reinterpret_cast<uintptr_t>(unit))&&unit[0x50]==12,"structured native exception restores scoped alpha before propagation");vertexThrows131=false;
 vertexChanges131=true;
#ifdef _WIN64
 VertexColorCall131(reinterpret_cast<uintptr_t>(unit));
#else
 VertexColorCall131(reinterpret_cast<uintptr_t>(unit),nullptr);
#endif
 check(unit[0x50]==77,"map-side alpha change during callback retains precedence");vertexChanges131=false;vertexEnabled131=false;
 alignas(uintptr_t) unsigned char root[256]{},frames[3][32]{},models[3][64]{};MenuFogVertexProfile131 p{};const uintptr_t base=1;p.glueTable=0x1233;p.frames=0x40;p.speed=0x20;p.modelGet=reinterpret_cast<uintptr_t>(ModelGetStub131)-base;
 *reinterpret_cast<uintptr_t*>(root)=0x1234;for(unsigned i=0;i<3;++i){*reinterpret_cast<uintptr_t*>(root+0x40+i*sizeof(uintptr_t))=reinterpret_cast<uintptr_t>(frames[i]);*reinterpret_cast<uintptr_t*>(frames[i]+sizeof(uintptr_t))=reinterpret_cast<uintptr_t>(models[i]);*reinterpret_cast<float*>(models[i]+0x20)=float(i+1);}
 glueSpeeds131={};glueBase131=base;glueProfile131=&p;glueThread131=GetCurrentThreadId();glueOriginal131=reinterpret_cast<uintptr_t>(GlueStub131);glueEnabled131=true;
#ifdef _WIN64
 const auto forwarded=GlueUpdateCall131(reinterpret_cast<uintptr_t>(root),.25f,123.f,99);
#else
 const auto forwarded=GlueUpdateCall131(reinterpret_cast<uintptr_t>(root),nullptr,.25f,123.f,99);
#endif
 check(forwarded==321&&glueElapsedTest131==.25f&&glueTimeTest131==123.f&&glueContextTest131==99,"glue callback forwards exact elapsed/time/context and result");for(unsigned i=0;i<3;++i)check(*reinterpret_cast<float*>(models[i]+0x20)==100.f,"only three menu model speed leases accelerated");
 *reinterpret_cast<float*>(models[1]+0x20)=4.f;ApplyGlueSpeeds131(reinterpret_cast<uintptr_t>(root),true);ApplyGlueSpeeds131(reinterpret_cast<uintptr_t>(root),false);check(*reinterpret_cast<float*>(models[0]+0x20)==1.f&&*reinterpret_cast<float*>(models[1]+0x20)==4.f&&*reinterpret_cast<float*>(models[2]+0x20)==3.f,"disable restores captured speeds and subsequent native changes");glueEnabled131=false;glueProfile131=nullptr;glueOwner131=0;glueSpeeds131={};
 float point[3]{};check(FogPoint131(reinterpret_cast<FogPosition131>(FogPointStub131),0x1000,point)&&fogPointOwner131==0x1000+(sizeof(void*)==8?0x358:0x284)&&point[0]==10&&point[1]==20,"fog query uses verified unit position property, not the unit header");
 for(unsigned physical:{0u,23u,24u})for(unsigned owner:{0u,1u,24u})for(bool shared:{false,true})for(unsigned flags:{0u,0x10u,0x20u})for(unsigned unitVisible:{0u,~0u})for(unsigned terrainVisible:{0u,~0u})check(FogAlertPolicy131(physical,owner,0,shared,flags,unitVisible,terrainVisible)==(physical<24&&owner>0&&owner<24&&!shared&&!flags&&(!unitVisible||!terrainVisible)),"fog alerts exclude invalid/self/shared-vision/dead units and accept either visibility failure");
 for(unsigned action:{0u,1u,2u,3u,4u})for(unsigned player:{0u,23u,24u})for(unsigned count:{0u,1u,12u,13u})check(FogPacketAccept131(action,player,count)==((action==1||action==3)&&player<24&&count>0&&count<=12),"selection packet acceptance matches action/player/count boundaries");
 alignas(uintptr_t) unsigned char packet[0x48]{};std::array<DropObjectId,12> ids{};for(unsigned i=0;i<12;++i)ids[i]={i+1,7};constexpr bool wide=sizeof(uintptr_t)==8;packet[wide?0x29:0x15]=2;packet[wide?0x30:0x18]=1;*reinterpret_cast<unsigned*>(packet+(wide?0x38:0x1c))=12;*reinterpret_cast<uintptr_t*>(packet+(wide?0x40:0x20))=reinterpret_cast<uintptr_t>(ids.data());FogPacket131 parsed;
 check(ReadFogPacket131(reinterpret_cast<uintptr_t>(packet),parsed)&&parsed.slot==2&&parsed.count==12&&parsed.ids[11]==ids[11],"native x86/x64 packet fields decode all twelve immutable IDs");ClearFogEvents131();FogClickCapture131(reinterpret_cast<void*>(1));check(fogEvents131.empty(),"disabled observer does not read invalid packet memory");fogWorld131=123;for(unsigned i=0;i<20;++i)FogClickCapture131(packet);check(fogEvents131.size()==96,"passive event queue bounded under selection bursts");check(packet[wide?0x30:0x18]==1&&ids[0].low==1,"observer preserves packet and unit IDs");ClearFogEvents131();check(fogEvents131.empty()&&!fogWorld131,"match reset clears observer queue/world");
 FogRecent131 recent;FogClickEvent131 e{2,{1,2},123,100};check(recent.Accept(e,100)&&!recent.Accept(e,2099)&&recent.Accept(e,2100),"duplicate fog notices suppressed for two seconds");e.world=456;check(recent.Accept(e,2101),"new world cannot inherit previous world deduplication");
 MenuFogVertex131 service;p.reg=reinterpret_cast<uintptr_t>(FogAddStub131)-base;p.unreg=reinterpret_cast<uintptr_t>(FogRemoveStub131)-base;service.base=base;service.profile=&p;check(service.Register(true,123)&&service.Register(true,123)&&fogAddsTest131==1,"one observer registration per world, no repeated register");check(service.Register(true,456)&&fogAddsTest131==2&&fogRemovesTest131==1,"world replacement removes old observer before registering");check(service.Register(false,0)&&fogRemovesTest131==2&&!fogWorld131,"disable unregisters observer and stops captures");
 for(unsigned build:{12164u,12173u}){const auto* profile=MenuFogVertexProfileFor131(build);check(profile&&profile->alpha&&profile->speed&&profile->reg&&profile->unreg,"independent color/glue/event profile exists");size_t size=0;for(size_t i=0;i<profile->count;++i)size=std::max(size,size_t(profile->code[i].rva+profile->code[i].length));auto bytes=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));check(bytes!=nullptr,"isolated profile image allocated");if(!bytes)continue;const uintptr_t imageBase=reinterpret_cast<uintptr_t>(bytes);
  for(size_t i=0;i<profile->count;++i){const auto& c=profile->code[i];memcpy(bytes+c.rva,c.bytes,c.length);for(size_t j=0;j<c.relocCount;++j){unsigned v=0;memcpy(&v,bytes+c.rva+c.reloc[j],4);v+=unsigned(imageBase);memcpy(bytes+c.rva+c.reloc[j],&v,4);}}
  check(MenuFogVertexGuard131(imageBase,*profile),"all captured color/glue/model/packet signatures accept PE relocations");for(size_t i=0;i<profile->count;++i){bytes[profile->code[i].rva]^=1;check(!MenuFogVertexGuard131(imageBase,*profile),"mutation in any signature range refuses profile");bytes[profile->code[i].rva]^=1;}VirtualFree(bytes,0,MEM_RELEASE);
 }
 check(!MenuFogVertexProfileFor131(999),"unsupported client profile refused");unsigned controls=0;for(const auto& d:kToolMenuControls)if(MenuFogVertexControl131(d.section,d.key))++controls;check(controls==3,"exactly three menu controls enabled");
}
