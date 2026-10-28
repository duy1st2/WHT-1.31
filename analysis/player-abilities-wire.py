from pathlib import Path
r=Path('WHT-1.31');p=r/'native/player_units131.hpp';s=p.read_text().replace('resource_command131.hpp','resource_commands131.hpp');p.write_text(s)
for f in ['version131.cpp','version64.cpp']:
 p=r/'native'/f;s=p.read_text(encoding='utf-8');s=s.replace('#include "unit_name131.hpp"','#include "unit_name131.hpp"\n#include "player_units131.hpp"')
 x=f=='version131.cpp';cmd='command' if x else 'cmd';base='base131' if x else 'base64';profile='profile131' if x else 'profile64';settings='settings131' if x else 'settings64';folder='folder131' if x else 'folder64';ui='currentUi131' if x else 'ui64';allowed='Allowed131()' if x else 'Allowed64()'
 # x86 authorization is checked centrally before these gameplay commands.
 gate=settings if x else settings+'&&'+allowed
 line=f' if({cmd}==L"#addall"||{cmd}==L"#raddall"){{std::wstring args;std::getline(in,args);std::string report;const bool ok={gate}&&wht::PlayerAbilitiesCommand131({base},{profile}->build,{cmd}==L"#addall",args,report);if(report.empty())report="Player Abilities: settings or session disabled";wht::Log({folder},report);wht::ShowNotification131({base},{profile}->build,{ui},report);return ok;}}'
 anchor=next(l for l in s.splitlines() if '==L"#ufinds")' in l);s=s.replace(anchor,anchor+'\n'+line);p.write_text(s,encoding='utf-8')
s=(r/'tests/unit-name-tests.cpp').read_text(encoding='utf-8');s=s[:s.index(' for(const auto& p:wht::unitNameProfiles131){',s.index(' for(const auto& p:wht::unitNameProfiles131){')+1)]
s=s.replace('unit_name131.hpp','player_units131.hpp').replace('unitNameProfiles131','playerUnitsProfiles131').replace('UnitNameCode131','PlayerUnitsCode131')
defs=r'''
bool __cdecl Visit(uintptr_t unit,void* context){auto& ids=*static_cast<std::vector<uintptr_t>*>(context);ids.push_back(unit);return ids.size()<3;}
void __cdecl MockEnumerate(uint32_t type,wht::PlayerUnitVisit131 visit,void* context,bool children){Check(type==0x2b773375&&!children,"exact CUnit class and no derived traversal");for(uintptr_t i=1;i<=10;++i)if(!visit(i,context))break;}
'''
s=s.replace('int main(){try{',defs+'\nint main(){try{')
s+=r'''
 std::vector<uintptr_t> seen;Check(wht::InvokePlayerUnits131(MockEnumerate,Visit,&seen)&&seen==std::vector<uintptr_t>({1,2,3}),"cdecl native enum and callback ABI including early stop");Check(!wht::InvokePlayerUnits131(nullptr,Visit,&seen)&&!wht::InvokePlayerUnits131(MockEnumerate,nullptr,&seen),"invalid API no call");
 std::vector<wht::RemoveUnitSource131> units;for(unsigned i=0;i<3;++i){wht::RemoveUnitSource131 u;u.unit=100+i;u.world=1;u.agent=2+i;u.id={3+i,4};u.local=0;units.push_back(u);}
 unsigned checks=0,writes=0,failCheck=0,failWrite=0;std::string report;
 auto valid=[&](const auto&){return ++checks!=failCheck;};auto apply=[&](const auto&,std::string& detail){++writes;if(writes==failWrite){detail="native result not confirmed";return false;}return true;};
 auto run=[&](){checks=writes=0;return wht::PlayerAbilitiesBatch131(units,valid,apply,report);};
 Check(run()&&writes==3&&report.find("3/3")!=report.npos,"all snapshotted units confirmed");
 failCheck=2;Check(!run()&&!writes&&report.find("no action taken")!=report.npos,"whole roster preflight before first write");failCheck=5;Check(!run()&&writes==1&&report.find("0/3")!=report.npos,"ownership/world invalidated after callback stops without false confirmation");failCheck=0;
 failWrite=2;Check(!run()&&writes==2&&report.find("1/3")!=report.npos&&report.find("may have applied")!=report.npos,"partial failure no retry no rollback and truthful count");failWrite=0;
 units[1].id=units[0].id;Check(!run()&&!writes,"duplicate identity refused before mutation");units.clear();Check(run()&&!writes&&report.find("0/0")!=report.npos,"verified empty owned snapshot succeeds with zero");units.resize(8193);Check(!run()&&!checks&&!writes,"oversized snapshot cannot partially modify");
 wht::playerAbilitiesBusy131.test_and_set();Check(!wht::PlayerAbilitiesCommand131(0,12164,true,L"AHhb",report)&&report.find("already running")!=report.npos,"batch reentry rejected before native access");wht::playerAbilitiesBusy131.clear();Check(!wht::PlayerAbilitiesCommand131(0,12164,true,L"AHhb extra",report)&&report.find("Usage")!=report.npos,"bad rawcode args before access");Check(!wht::playerAbilitiesBusy131.test_and_set(),"bad args do not take batch flag");wht::playerAbilitiesBusy131.clear();
 puts("player unit ability batch contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
''';(r/'tests/player-abilities-tests.cpp').write_text(s,encoding='utf-8')
p=r/'CMakeLists.txt';s=p.read_text(encoding='utf-8');s+='''
if(BUILD_TESTING)
 add_executable(WHT131PlayerAbilitiesTests tests/player-abilities-tests.cpp)
 target_link_libraries(WHT131PlayerAbilitiesTests PRIVATE wht_common user32)
 target_compile_options(WHT131PlayerAbilitiesTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME player-abilities-contracts COMMAND WHT131PlayerAbilitiesTests)
endif()
''';p.write_text(s,encoding='utf-8')
