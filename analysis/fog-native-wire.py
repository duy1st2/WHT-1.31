from pathlib import Path
r=Path('WHT-1.31')
for f in ['version131.cpp','version64.cpp']:
 p=r/'native'/f;s=p.read_text(encoding='utf-8');s=s.replace('#include "map_trade131.hpp"','#include "map_trade131.hpp"\n#include "fog_modifier131.hpp"');x=f=='version131.cpp';suffix='131' if x else '64';cmd='command' if x else 'cmd';base='base'+suffix;profile='profile'+suffix;settings='settings'+suffix;folder='folder'+suffix;ui='currentUi131' if x else 'ui64';allowed='Allowed'+suffix+'()'
 anchor=next(l for l in s.splitlines() if '==L"#mtrade")' in l)
 line=f' if({cmd}==L"#mh"){{std::wstring args;std::getline(in,args);std::string report;const bool ok=fogModifier{suffix}.Command({base},{profile}->build,{ui},{allowed}&&{settings},args,report);wht::Log({folder},report);wht::ShowNotification131({base},{profile}->build,{ui},report);return ok;}}'
 s=s.replace(anchor,anchor+'\n'+line)
 # Place global owner beside existing synchronous native command state.
 marker=next(l for l in s.splitlines() if 'noCooldown'+suffix+';' in l)
 s=s.replace(marker,marker+'\nwht::FogModifierFeature131 fogModifier'+suffix+';')
 if x:
  anchor=' if(currentUi131!=ui){DestroyUi131();currentUi131=ui;}'
 else:
  anchor='const auto ui=CurrentUi64();if(ui!=ui64){Destroy64();ui64=ui;}'
 assert anchor in s
 if x:s=s.replace(anchor,anchor+'\n {std::string fogReport;if(!fogModifier131.Update(base131,profile131->build,currentUi131,Allowed131()&&settings131,fogReport)&&!fogReport.empty())wht::Log(folder131,fogReport);}')
 else:s=s.replace(anchor,anchor+'{std::string fogReport;if(!fogModifier64.Update(base64,profile64->build,ui64,Allowed64()&&settings64,fogReport)&&!fogReport.empty())wht::Log(folder64,fogReport);}')
 p.write_text(s,encoding='utf-8')
# Full captured guards plus exact native virtual-slot checks.
s=(r/'tests/unit-name-tests.cpp').read_text(encoding='utf-8');s=s[:s.index(' for(const auto& p:wht::unitNameProfiles131){',s.index(' for(const auto& p:wht::unitNameProfiles131){')+1)];s=s.replace('unit_name131.hpp','fog_modifier131.hpp').replace('unitNameProfiles131','fogNativeProfiles131').replace('UnitNameCode131','FogNativeCode131')
s=s.replace('Fixture image(size+64);','size=(std::max)(size,size_t((std::max)(p.fogTable,p.rectTable)+26*sizeof(uintptr_t)));const auto* rp=wht::ResourceProfileFor131(p.build);size=(std::max)(size,size_t(rp->playerTable+26*sizeof(uintptr_t)));Fixture image(size+64);')
s=s.replace(' Check(wht::FogNativeCode131(base,p)', ''' for(const auto slot:std::vector<std::pair<uintptr_t,uintptr_t>>{{p.fogTable+24*sizeof(uintptr_t),p.configure},{p.fogTable+23*sizeof(uintptr_t),p.fogDelete},{p.rectTable+23*sizeof(uintptr_t),p.rectDelete},{rp->playerTable+25*sizeof(uintptr_t),p.ownerGetter}}){uintptr_t target=0;input.seekg(slot.first);input.read(reinterpret_cast<char*>(&target),sizeof(target));Check(target==capturedBase+slot.second,"actual captured native virtual slot");target=base+slot.second;memcpy(image.data+slot.first,&target,sizeof(target));}
 Check(wht::FogNativeCode131(base,p)''')
s=s.replace('int main(){try{',r'''
uint32_t seenPlayer=0,seenState=0,seenRect=0,seenShared=0,seenAfter=0,seenAction=0;
uint32_t __cdecl MockZero(){return 77;}
uint32_t __cdecl MockCreate(uint32_t player,uint32_t state,uint32_t rect,uint32_t shared,uint32_t after){seenPlayer=player;seenState=state;seenRect=rect;seenShared=shared;seenAfter=after;return 88;}
void __cdecl MockAction(uint32_t handle){seenAction=handle;}
uintptr_t __cdecl MockResolve(uint32_t handle){return handle==88?uintptr_t(0x12345678):0;}
int main(){try{''')
s+=r'''
 for(const auto& p:wht::fogNativeProfiles131){
  alignas(uintptr_t) unsigned char object[256]{};const auto ptr=reinterpret_cast<uintptr_t>(object);const uintptr_t base=0x100000;uintptr_t table=base+p.fogTable;memcpy(object,&table,sizeof(table));const uint32_t id[2]={11,22};memcpy(object+(sizeof(uintptr_t)==8?0x18:0xc),id,sizeof(id));uint32_t flags=0xcc,mask=4;memcpy(object+(sizeof(uintptr_t)==8?0x38:0x20),&flags,4);memcpy(object+(sizeof(uintptr_t)==8?0x3c:0x24),&mask,4);float coords[4]={-4096,-2048,4096,2048};memcpy(object+(sizeof(uintptr_t)==8?0x50:0x30),coords,sizeof(coords));
  wht::FogObjectFields131 fields;Check(wht::ReadFogObjectFields131(base,p,ptr,false,fields)&&fields.low==11&&fields.high==22&&fields.flags==flags&&fields.mask==4&&!memcmp(fields.coordinates,coords,sizeof(coords)),"native modifier identity/configuration offsets");
  ++table;memcpy(object,&table,sizeof(table));Check(!wht::ReadFogObjectFields131(base,p,ptr,false,fields),"foreign class refused");table=base+p.rectTable;memcpy(object,&table,sizeof(table));memcpy(object+(sizeof(uintptr_t)==8?0x40:0x24),coords,sizeof(coords));Check(wht::ReadFogObjectFields131(base,p,ptr,true,fields),"exact rectangle native coordinates");
  coords[0]=NAN;memcpy(object+(sizeof(uintptr_t)==8?0x40:0x24),coords,sizeof(coords));Check(!wht::ReadFogObjectFields131(base,p,ptr,true,fields),"invalid bounds refused");
 }
 uint32_t result=0;Check(wht::InvokeFogZero131(MockZero,result)&&result==77,"cdecl zero-argument handle API");Check(wht::InvokeFogCreate131(MockCreate,11,22,result)&&result==88&&seenPlayer==11&&seenState==4&&seenRect==22&&seenShared==1&&seenAfter==0,"cdecl actual player/rect handles and visible shared rectangle arguments");Check(!wht::InvokeFogCreate131(MockCreate,0,22,result),"invalid input handle no call");Check(wht::InvokeFogAction131(MockAction,88)&&seenAction==88,"cdecl action handle ABI");Check(wht::InvokeFogResolve131(MockResolve,88)==uintptr_t(0x12345678),"cdecl typed native object result width");
 bool enabled=false;Check(wht::ParseFogToggle131(L"",false,enabled)&&enabled&&wht::ParseFogToggle131(L"",true,enabled)&&!enabled,"no-argument toggle");Check(wht::ParseFogToggle131(L" -1 ",false,enabled)&&enabled&&wht::ParseFogToggle131(L" 0 ",true,enabled)&&!enabled,"reference integer nonzero/zero semantics");for(const auto* bad:{L"on",L"1 extra",L"1\n",L"2147483648",L"1.0"})Check(!wht::ParseFogToggle131(bad,false,enabled),"invalid args reject instead of accidental toggle");Check(!wht::ParseFogToggle131(std::wstring(L"1\0",2),false,enabled),"embedded NUL refused");
 puts("fog modifier native profile and ABI contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
''';(r/'tests/fog-native-tests.cpp').write_text(s,encoding='utf-8')
p=r/'CMakeLists.txt';s=p.read_text(encoding='utf-8');s+='''
if(BUILD_TESTING)
 add_executable(WHT131FogNativeTests tests/fog-native-tests.cpp)
 target_link_libraries(WHT131FogNativeTests PRIVATE wht_common user32)
 target_compile_options(WHT131FogNativeTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME fog-modifier-native-contracts COMMAND WHT131FogNativeTests)
endif()
''';p.write_text(s,encoding='utf-8')
