wht::ParityPatch129 parityLocalDelay129,parityLanDelay129,parityNetDelay129,parityStartDelay129;
bool parityHandled129=false;
wht::ParityPatch129 parityMapSize129[3];
using SkillCard129=int(__cdecl*)(uintptr_t,uintptr_t);
SkillCard129 originalInspectCard129=nullptr,originalInspectExtra129=nullptr;
bool inspectSkillsReady129=false,delayReady129=false,mapSizeReady129=false;
using ParityEndScene129=HRESULT(__thiscall*)(uintptr_t);
ParityEndScene129 originalParityEndScene129=nullptr;
thread_local wht::AmbientLight129 ambientLight129;
std::atomic<unsigned> requestedAmbient129{0};
using AmbientStateFn129=HRESULT(WINAPI*)(uintptr_t,DWORD,DWORD);
AmbientStateFn129 originalAmbientState129=nullptr;
std::atomic<uintptr_t> ambientDevice129{0};
thread_local DWORD engineAmbient129=0;
thread_local bool engineAmbientKnown129=false;
thread_local bool writingAmbient129=false;
HRESULT WINAPI ParityAmbientState129(uintptr_t device,DWORD state,DWORD value){
 const auto requested=requestedAmbient129.load();
 if(device==ambientDevice129.load()&&state==139){if(!writingAmbient129){engineAmbient129=value;engineAmbientKnown129=true;}
  if(requested)value=0xff000000|(requested<<16)|(requested<<8)|requested;
 }
 return originalAmbientState129(device,state,value);
}
uintptr_t RendererDevice129(uintptr_t renderer){
 __try{return *reinterpret_cast<uintptr_t*>(renderer+0x59c);}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
uintptr_t AmbientMethod129(uintptr_t device){
 __try{return *reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(device)+57*4);}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
HRESULT __fastcall ParityLightHook129(uintptr_t renderer,DWORD){
 const auto device=RendererDevice129(renderer);
 if(ambientDevice129.exchange(device)!=device){engineAmbientKnown129=false;ambientLight129={};}
 if(device&&!originalAmbientState129){auto target=reinterpret_cast<void*>(AmbientMethod129(device));
  if(target&&MH_CreateHook(target,ParityAmbientState129,reinterpret_cast<void**>(&originalAmbientState129))==MH_OK)
   if(MH_EnableHook(target)!=MH_OK){MH_RemoveHook(target);originalAmbientState129=nullptr;}
 }
 const auto requested=requestedAmbient129.load();
 if(!requested&&ambientLight129.active&&engineAmbientKnown129&&originalAmbientState129){originalAmbientState129(device,139,engineAmbient129);ambientLight129.active=false;}
 auto result=originalParityEndScene129(renderer);
 if(result==HRESULT(0x88760868)||result==HRESULT(0x88760869))ambientLight129={};
 else if(SUCCEEDED(result)){writingAmbient129=true;ambientLight129.Apply(renderer,requested);writingAmbient129=false;}
 return result;
}
using SaveCommandFn129=int(__cdecl*)(uintptr_t);
SaveCommandFn129 originalSaveCommand129=nullptr;
std::mutex paritySaveMutex129;
std::vector<unsigned char> paritySaveEvents129;
unsigned char SavePlayer129(uintptr_t command){
 unsigned char player=255;
 __try{if(command)player=*reinterpret_cast<unsigned char*>(command+0x15);}__except(EXCEPTION_EXECUTE_HANDLER){}
 return player;
}
int __cdecl ParitySaveHook129(uintptr_t command){
 auto player=SavePlayer129(command);
 if(player<24){std::lock_guard<std::mutex> lock(paritySaveMutex129);if(paritySaveEvents129.size()<128)paritySaveEvents129.push_back(player);}
 return originalSaveCommand129(command);
}
using ScriptLoadFn129=int(__cdecl*)(const char*,void**,DWORD*,DWORD,void*);
ScriptLoadFn129 originalScriptLoad129=nullptr;
thread_local bool scriptLoading129=false;
struct ScriptSettings129 {bool allowed=false,enable=false,load=false;std::wstring file,folder;};
std::mutex scriptSettingsMutex129;
ScriptSettings129 scriptSettings129;
struct NameSettings129 {bool allowed=false;std::string original,oldName,newName,hostName;};
std::mutex nameSettingsMutex129;
NameSettings129 nameSettings129;
using LobbyNameFn129=const char*(__cdecl*)(unsigned,unsigned);
LobbyNameFn129 originalLobbyName129=nullptr;
bool LobbyLocalId129(unsigned session,unsigned& id){
 __try{id=static_cast<unsigned char>(reinterpret_cast<char(__cdecl*)(unsigned)>(executableBase+0x1ac7e0)(session));return id!=255;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
const char* __cdecl ParityLobbyName129(unsigned id,unsigned session){
 const char* original=originalLobbyName129(id,session);unsigned local=255;
 if(!original||!LobbyLocalId129(session,local)||static_cast<unsigned char>(id)!=local)return original;
 thread_local std::string replacement;NameSettings129 settings;
 {std::lock_guard<std::mutex> guard(nameSettingsMutex129);settings=nameSettings129;}
 replacement=settings.allowed?(wht::playerNetwork129.IsHost()&&!settings.hostName.empty()?settings.hostName:settings.newName):"";
 return replacement.empty()?original:replacement.c_str();
}
using LobbySendFn129=int(__thiscall*)(uintptr_t,uintptr_t);
LobbySendFn129 originalLobbySend129=nullptr,originalChannelSend129=nullptr;
thread_local uintptr_t parityRoomLog129=0;
void ParityNotice129(const std::string&);
bool LobbyFrames129(uintptr_t owner,bool room,uintptr_t& edit,uintptr_t& log,const char*& text){
 __try{edit=*reinterpret_cast<uintptr_t*>(owner+(room?0x2b0:0x20c));log=*reinterpret_cast<uintptr_t*>(owner+(room?0x2ac:0x208));
  text=edit?*reinterpret_cast<const char**>(edit+0x1f4):nullptr;return edit&&log&&text&&strnlen(text,1024)<1024;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool RoomMessage129(uintptr_t log,const char* text){
 __try{if(!log)return false;reinterpret_cast<void(__thiscall*)(uintptr_t,const char*)>(executableBase+0x485110)(log,text);return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void ClearLobbyInput129(uintptr_t edit){
 __try{reinterpret_cast<void(__thiscall*)(uintptr_t,const char*,int)>(executableBase+0x481c70)(edit,"",1);}
 __except(EXCEPTION_EXECUTE_HANDLER){}
}
int LobbyCommand129(uintptr_t owner,uintptr_t event,bool room){
 auto original=room?originalLobbySend129:originalChannelSend129;
 uintptr_t edit=0,log=0;const char* text=nullptr;
 if(runtimeConfig.mode!=wht::RunMode::Local||!LobbyFrames129(owner,room,edit,log,text)||*text!='#')return original(owner,event);
 auto previous=parityRoomLog129;parityRoomLog129=log;bool consumed=false;
 try{consumed=ToolCommand(wht::Wide(text))||parityHandled129;}catch(...){ParityNotice129("Command failed");consumed=true;}
 parityRoomLog129=previous;
 if(!consumed)return original(owner,event);ClearLobbyInput129(edit);return 1;
}
int __fastcall ParityLobbySend129(uintptr_t owner,DWORD,uintptr_t event){return LobbyCommand129(owner,event,true);}
int __fastcall ParityChannelSend129(uintptr_t owner,DWORD,uintptr_t event){return LobbyCommand129(owner,event,false);}
int __cdecl ParityScriptLoad129(const char* filename,void** buffer,DWORD* size,DWORD reserve,void* overlap){
 if(scriptLoading129||!buffer||!size||!wht::IsMapScript129(filename))return originalScriptLoad129(filename,buffer,size,reserve,overlap);
 ScriptSettings129 settings;{std::lock_guard<std::mutex> guard(scriptSettingsMutex129);settings=scriptSettings129;}
 if(!settings.allowed||(!settings.enable&&!settings.load))return originalScriptLoad129(filename,buffer,size,reserve,overlap);
 scriptLoading129=true;int result=0;
 try{
  if(settings.load){auto path=settings.file.empty()?settings.folder+L"\\war3map_ets.j":settings.file;
   if(std::filesystem::path(path).is_relative())path=settings.folder+L"\\"+path;
   const auto encoded=wht::Utf8(path);
   if(GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES){
    // The seven-argument SFileLoadFileEx adapter supplies its search flags
    // explicitly; absolute override files are loaded by the same allocator.
    using LoadEx=int(__cdecl*)(void*,const char*,void**,DWORD*,DWORD,DWORD,void*);
    result=reinterpret_cast<LoadEx>(executableBase+0x395d0)(nullptr,encoded.c_str(),buffer,size,reserve,1,overlap);
   }
   if(!result)wht::Log(settings.folder,"war3map: override unavailable; loading map script.");
  }
  if(!result)result=originalScriptLoad129(filename,buffer,size,reserve,overlap);
  if(result&&settings.enable&&*buffer&&*size){
   auto changed=wht::PatchDefeatCalls129(static_cast<char*>(*buffer),*size);
   wht::Log(settings.folder,"war3map: patched "+std::to_string(changed)+" defeat calls.");
   if(changed){std::ofstream out(std::filesystem::path(settings.folder)/L"war3map_ets.j",std::ios::binary);if(out)out.write(static_cast<char*>(*buffer),*size);}
  }
 }catch(...){wht::Log(settings.folder,"war3map: script override failed.");}
 scriptLoading129=false;return result;
}
int InspectCard129(uintptr_t unit,uintptr_t panel,bool extra){
 auto original=extra?originalInspectExtra129:originalInspectCard129;
 if(!panel||runtimeConfig.mode!=wht::RunMode::Local||!runtimeConfig.showUnitSkills||!wht::InspectableUnit129(executableBase,unit))return original?original(unit,panel):0;
 __try{const auto submenu=*reinterpret_cast<uintptr_t*>(panel+0x1c8);
  uint32_t order=intptr_t(submenu)>0?*reinterpret_cast<uint32_t*>(submenu+8):0xd0142;
  reinterpret_cast<void(__thiscall*)(uintptr_t,uint32_t)>(executableBase+(extra?0x28e7b0:0x28e700))(unit,order);return 1;
 }__except(EXCEPTION_EXECUTE_HANDLER){return original?original(unit,panel):0;}
}
int __cdecl InspectCardNormal129(uintptr_t unit,uintptr_t panel){return InspectCard129(unit,panel,false);}
int __cdecl InspectCardExtra129(uintptr_t unit,uintptr_t panel){return InspectCard129(unit,panel,true);}
void ApplyParityPatches129(){
 const bool enabled=runtimeConfig.mode==wht::RunMode::Local;
 const bool delay=enabled&&commandSettingsActive;
 if(delayReady129){parityLocalDelay129.Apply(runtimeConfig.localDelay,delay);parityLanDelay129.Apply(runtimeConfig.lanDelay,delay);parityNetDelay129.Apply(runtimeConfig.netDelay,delay);
  uint32_t bits=0;memcpy(&bits,&runtimeConfig.startDelay,4);parityStartDelay129.Apply(bits,delay);}
 if(mapSizeReady129)for(auto& p:parityMapSize129)p.Apply(0x7ffe0000,enabled&&runtimeConfig.unlockMap);
}
void InstallParityHooks129(){
 const auto b=executableBase;
 auto names=(void*)(b+0x1ace10);const BYTE nameSignature[]={0x55,0x8b,0xec,0x6a,0x0d,0xe8};
 if(!memcmp(names,nameSignature,sizeof(nameSignature))&&!memcmp((void*)(b+0x1ac7e0),nameSignature,sizeof(nameSignature))&&MH_CreateHook(names,ParityLobbyName129,reinterpret_cast<void**>(&originalLobbyName129))==MH_OK)
  if(MH_EnableHook(names)!=MH_OK){MH_RemoveHook(names);originalLobbyName129=nullptr;}
 auto installRoom=[&](uintptr_t rva,bool room){auto target=(void*)(b+rva);const BYTE entry[]={0x55,0x8b,0xec,0x6a,0xff,0x68};
  auto& original=room?originalLobbySend129:originalChannelSend129;
  uintptr_t field=room?0x17bb0c:0x17bcdf;const BYTE readRoom[]={0x8b,0x8e,0xb0,2,0,0},readChannel[]={0x8b,0x8f,0x0c,2,0,0};
  if(!memcmp(target,entry,sizeof(entry))&&!memcmp((void*)(b+field),room?readRoom:readChannel,6)&&
     !memcmp((void*)(b+0x4804c0),"\x8b\x81\xf4\x01\0\0\xc3",7)&&MH_CreateHook(target,room?(void*)ParityLobbySend129:(void*)ParityChannelSend129,reinterpret_cast<void**>(&original))==MH_OK)
   if(MH_EnableHook(target)!=MH_OK){MH_RemoveHook(target);original=nullptr;}
 };installRoom(0x17bad0,true);installRoom(0x17bcb0,false);
 const BYTE lightSignature[]={0x55,0x8b,0xec,0x83,0xec,0x28,0xa1};
 auto light=(void*)(b+0x3dc400);
 if(!memcmp(light,lightSignature,sizeof(lightSignature))&&!memcmp((void*)(b+0x3dc4b7),"\x8b\x86\x9c\x05\x00\x00",6)&&!memcmp((void*)(b+0x3dc4c0),"\xff\x91\xa8\0\0\0",6)&&MH_CreateHook(light,ParityLightHook129,reinterpret_cast<void**>(&originalParityEndScene129))==MH_OK)
  if(MH_EnableHook(light)!=MH_OK){MH_RemoveHook(light);originalParityEndScene129=nullptr;}
 const BYTE saveSignature[]={0x55,0x8b,0xec,0x6a,0xff,0x68};auto save=(void*)(b+0xd66a0);
 if(!memcmp(save,saveSignature,sizeof(saveSignature))&&!memcmp((void*)(b+0xd66cd),"\x8b\x75\x08\x8a\x46\x15",6)&&MH_CreateHook(save,ParitySaveHook129,reinterpret_cast<void**>(&originalSaveCommand129))==MH_OK)
  if(MH_EnableHook(save)!=MH_OK){MH_RemoveHook(save);originalSaveCommand129=nullptr;}
 const BYTE loadSignature[]={0x55,0x8b,0xec,0x56,0x8b,0x35};auto load=(void*)(b+0x39570);
 if(!memcmp(load,loadSignature,sizeof(loadSignature))&&*reinterpret_cast<uintptr_t*>(b+0x39576)==b+0xd2ed28&&!memcmp((void*)(b+0x395d0),"\x55\x8b\xec\x6a\x00",5)&&MH_CreateHook(load,ParityScriptLoad129,reinterpret_cast<void**>(&originalScriptLoad129))==MH_OK)
  if(MH_EnableHook(load)!=MH_OK){MH_RemoveHook(load);originalScriptLoad129=nullptr;}
 const BYTE delaySignature[]={0xb8,0x64,0,0,0,0xc3};
 delayReady129=!memcmp((void*)(b+0x52ec50),delaySignature,6)&&!memcmp((void*)(b+0x5335b0),delaySignature,6)&&!memcmp((void*)(b+0x530270),delaySignature,6)&&
  *reinterpret_cast<uintptr_t*>(b+0xad4ae8)==b+0x52ec50&&*reinterpret_cast<uintptr_t*>(b+0xad4bdc)==b+0x530270&&*reinterpret_cast<uintptr_t*>(b+0xad4c8c)==b+0x5335b0&&
  !memcmp((void*)(b+0x17cf93),"\xf3\x0f\x10\x0d",4)&&*reinterpret_cast<uintptr_t*>(b+0x17cf97)==b+0xa8dfe0&&
  parityLocalDelay129.Probe(b+0x52ec51,100)&&parityLanDelay129.Probe(b+0x5335b1,100)&&parityNetDelay129.Probe(b+0x530271,100)&&parityStartDelay129.Probe(b+0xa8dfe0,0x40c00000);
 const uintptr_t caps[]={0x5376db,0x54516a,0x555838};mapSizeReady129=true;
 for(unsigned i=0;i<3;++i)if(*reinterpret_cast<BYTE*>(b+caps[i])!=0x3d||!parityMapSize129[i].Probe(b+caps[i]+1,0x10000000))mapSizeReady129=false;
 const BYTE cardSignature[]={0x55,0x8b,0xec,0x8b,0x45,0x0c,0x85,0xc0,0x75,0x02,0x5d,0xc3};
 const BYTE normalEnum[]={0x55,0x8b,0xec,0x56,0x8b,0xf1,0xe8};
 const BYTE extraEnum[]={0x55,0x8b,0xec,0x8b,0x81,0xec,3,0,0};
 auto first=(void*)(b+0x22ec70),second=(void*)(b+0x22ecb0);
 if(!memcmp(first,cardSignature,sizeof(cardSignature))&&!memcmp(second,cardSignature,sizeof(cardSignature))&&
  !memcmp((void*)(b+0x28e700),normalEnum,sizeof(normalEnum))&&!memcmp((void*)(b+0x28e7b0),extraEnum,sizeof(extraEnum))){
  bool made1=MH_CreateHook(first,InspectCardNormal129,reinterpret_cast<void**>(&originalInspectCard129))==MH_OK;
  bool made2=made1&&MH_CreateHook(second,InspectCardExtra129,reinterpret_cast<void**>(&originalInspectExtra129))==MH_OK;
  inspectSkillsReady129=made2&&MH_EnableHook(first)==MH_OK&&MH_EnableHook(second)==MH_OK;
  if(!inspectSkillsReady129){if(made1){MH_DisableHook(first);MH_RemoveHook(first);}if(made2){MH_DisableHook(second);MH_RemoveHook(second);}originalInspectCard129=originalInspectExtra129=nullptr;}
 }
 wht::Log(runtimeFolder,"WHT 1.29 parity hooks: delay="+std::to_string(delayReady129)+" map-size="+std::to_string(mapSizeReady129)+" inspect-skills="+std::to_string(inspectSkillsReady129)+" ambient="+std::to_string(originalParityEndScene129!=nullptr)+" save="+std::to_string(originalSaveCommand129!=nullptr)+" map-script="+std::to_string(originalScriptLoad129!=nullptr));
}
