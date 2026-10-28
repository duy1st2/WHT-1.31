from pathlib import Path
r=Path('WHT-1.31')
for f in ['version131.cpp','version64.cpp']:
 p=r/'native'/f;s=p.read_text(encoding='utf-8');s=s.replace('#include "player_units131.hpp"','#include "player_units131.hpp"\n#include "map_trade131.hpp"');x=f=='version131.cpp';cmd='command' if x else 'cmd';base='base131' if x else 'base64';profile='profile131' if x else 'profile64';settings='settings131' if x else 'settings64';folder='folder131' if x else 'folder64';ui='currentUi131' if x else 'ui64';gate=settings if x else settings+'&&Allowed64()'
 line=f' if({cmd}==L"#mtrade"){{std::wstring args;std::getline(in,args);std::string report;const bool ok={gate}&&wht::MapTradeCommand131(args,[&](wht::MapTradeSource131& out){{return wht::ReadMapTrade131({base},{profile}->build,out);}},[&](const wht::MapTradeSource131& source){{return wht::ClearMapTradeNative131({base},{profile}->build,source);}},report);if(report.empty())report="Trade map flag: settings or session disabled";wht::Log({folder},report);wht::ShowNotification131({base},{profile}->build,{ui},report);return ok;}}'
 anchor=next(l for l in s.splitlines() if '==L"#ufinds")' in l);s=s.replace(anchor,anchor+'\n'+line);p.write_text(s,encoding='utf-8')
s=(r/'tests/unit-name-tests.cpp').read_text(encoding='utf-8');s=s[:s.index(' for(const auto& p:wht::unitNameProfiles131){',s.index(' for(const auto& p:wht::unitNameProfiles131){')+1)];s=s.replace('unit_name131.hpp','map_trade131.hpp').replace('unitNameProfiles131','mapTradeProfiles131').replace('UnitNameCode131','MapTradeCode131')
s+=r'''
 for(const auto& p:wht::mapTradeProfiles131){
  alignas(uintptr_t) unsigned char world[512]{},setup[256]{};const uintptr_t base=0x100000,wp=reinterpret_cast<uintptr_t>(world),sp=reinterpret_cast<uintptr_t>(setup);uintptr_t table=base+p.setupTable;memcpy(setup,&table,sizeof(table));memcpy(world+p.setupOffset,&sp,sizeof(sp));uint32_t flags=0xa5a5f7ff;memcpy(setup+p.flagsOffset,&flags,4);
  uintptr_t readSetup=0;uint32_t actual=0;Check(wht::ReadMapTradeFields131(base,p,wp,readSetup,actual)&&readSetup==sp&&actual==flags,"exact CMapSetup identity and native map flag offset");
  Check(wht::ClearMapTradeField131(base,p,sp,flags),"native flag bit cleared");memcpy(&actual,setup+p.flagsOffset,4);Check(actual==(flags&~0x100u)&&((actual^flags)==0x100u),"all other32-bit map flags preserved");
  Check(!wht::ClearMapTradeField131(base,p,sp,flags),"changed source compare/exchange refused");memcpy(&actual,setup+p.flagsOffset,4);Check(actual==(flags&~0x100u),"conflict cannot overwrite flags");
  ++table;memcpy(setup,&table,sizeof(table));Check(!wht::ReadMapTradeFields131(base,p,wp,readSetup,actual)&&!wht::ClearMapTradeField131(base,p,sp,actual),"wrong setup class refused for reads and writes");
 }
 wht::MapTradeSource131 value;value.world=1;value.setup=2;value.local=0;value.flags=0x40100;unsigned reads=0,writes=0,change=0;bool unavailable=false,clearOK=true;std::string report;
 auto read=[&](wht::MapTradeSource131& out){out=value;++reads;if(unavailable)return false;if(reads==2)switch(change){case 1:++out.world;break;case 2:++out.setup;break;case 3:++out.local;break;case 4:out.flags^=2;break;}if(reads==3){if(change==5)++out.world;if(change==6)out.flags|=0x100;if(change==7)out.flags^=2;}return true;};
 auto clear=[&](const wht::MapTradeSource131& before){++writes;Check(before.flags==value.flags,"exact source passed to writer");if(!clearOK)return false;value.flags&=~0x100u;return true;};
 auto run=[&](const std::wstring& args){reads=writes=0;return wht::MapTradeCommand131(args,read,clear,report);};
 Check(run(L" ")&&writes==1&&reads==3&&value.flags==0x40000&&report=="Trade restriction disabled","confirmed map restriction clear");Check(run(L"")&&!writes&&reads==2&&report.find("already disabled")!=report.npos,"already clear is read-only");
 for(const auto* args:{L"1",L"off",L"\n"})Check(!run(args)&&!reads&&!writes,"no extra args or newline allowed");Check(!run(std::wstring(L"\0",1))&&!reads,"NUL rejected");
 for(change=1;change<=4;++change){value.flags=0x40100;Check(!run(L"")&&!writes,"unstable world setup player or flags no action");}change=0;unavailable=true;Check(!run(L"")&&!writes&&reads==1,"unavailable native context no writes");unavailable=false;
 clearOK=false;Check(!run(L"")&&writes==1&&value.flags==0x40100,"compare/exchange refusal not reported as success");clearOK=true;
 for(change=5;change<=7;++change){value.flags=0x40100;Check(!run(L"")&&writes==1&&report.find("not confirmed")!=report.npos,"world change or unexpected post-write bits reported honestly");}
 puts("map trade native flag contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
''';(r/'tests/map-trade-tests.cpp').write_text(s,encoding='utf-8')
p=r/'CMakeLists.txt';s=p.read_text(encoding='utf-8');s+='''
if(BUILD_TESTING)
 add_executable(WHT131MapTradeTests tests/map-trade-tests.cpp)
 target_link_libraries(WHT131MapTradeTests PRIVATE wht_common user32)
 target_compile_options(WHT131MapTradeTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME map-trade-contracts COMMAND WHT131MapTradeTests)
endif()
''';p.write_text(s,encoding='utf-8')
