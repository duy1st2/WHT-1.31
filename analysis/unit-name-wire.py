from pathlib import Path
r=Path('WHT-1.31')
for fn in ['version131.cpp','version64.cpp']:
 p=r/'native'/fn;s=p.read_text(encoding='utf-8');s=s.replace('#include "unit_find131.hpp"','#include "unit_find131.hpp"\n#include "unit_name131.hpp"')
 line=next(x for x in s.splitlines() if 'if(' in x and '==L"#ufind")' in x)
 new=line.replace('#ufind','#ufinds').replace('UnitFindCommand131','UnitNameCommand131').replace('UnitFindSource131','UnitNameSource131').replace('ReadUnitFind131','ReadUnitName131');s=s.replace(line,line+'\n'+new);p.write_text(s,encoding='utf-8')
p=r/'CMakeLists.txt';s=p.read_text(encoding='utf-8');s+='''\nif(BUILD_TESTING)
 add_executable(WHT131UnitNameTests tests/unit-name-tests.cpp)
 target_link_libraries(WHT131UnitNameTests PRIVATE wht_common user32)
 target_compile_options(WHT131UnitNameTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME unit-name-contracts COMMAND WHT131UnitNameTests)
endif()
''';p.write_text(s,encoding='utf-8')
s=(r/'tests/unit-model-tests.cpp').read_text();s=s[:s.index(' for(const auto& p:wht::unitModelProfiles131){',s.index(' for(const auto& p:wht::unitModelProfiles131){')+1)]
s=s.replace('unit_model131.hpp','unit_name131.hpp').replace('unitModelProfiles131','unitNameProfiles131').replace('UnitModelCode131','UnitNameCode131')
s+=r'''
 for(const auto& p:wht::unitNameProfiles131){
  unsigned char unit[512]{};char output[260]{};const auto u=reinterpret_cast<uintptr_t>(unit);
  Check(!wht::ReadUnitNameCache131(u,p,output),"empty cache unavailable without initializing game");
  memcpy(unit+p.nameOffset,"Footman",8);Check(wht::ReadUnitNameCache131(u,p,output)&&!strcmp(output,"Footman"),"inline native name cache exact offset");
  memset(unit+p.nameOffset,'a',260);Check(!wht::ReadUnitNameCache131(u,p,output),"unterminated native cache refused at260");unit[p.nameOffset+259]=0;Check(wht::ReadUnitNameCache131(u,p,output)&&strlen(output)==259,"native maximum259");
 }
 std::wstring query,decoded;std::string report;
 Check(wht::ParseUnitNameQuery131(L"  foot MAN  ",query)&&query==L"foot MAN","multiword query trim");
 for(const auto* bad:{L"",L"   ",L"x\n",L"x\r",L"|cffff0000x",L"\xD800"})Check(!wht::ParseUnitNameQuery131(bad,query),"malformed query no game reads");Check(!wht::ParseUnitNameQuery131(std::wstring(L"x\0y",3),query),"embedded NUL refused");Check(!wht::ParseUnitNameQuery131(std::wstring(260,L'a'),query),"query byte cap");
 Check(!wht::DecodeUnitName131(std::string("\xc0\xaf",2),decoded)&&!wht::DecodeUnitName131("name\n",decoded),"invalid UTF8 and control native name refused");
 Check(wht::PlainUnitName131(L"|cffff0000Footman|r")==L"Footman"&&wht::PlainUnitName131(L"A|nB")==L"A B","display markup excluded from search");
 Check(wht::UnitNameContains131(L"Chevalier Élite",L"ÉLITE")&&!wht::UnitNameContains131(L"Footman",L"Knight"),"ordinal case-insensitive Unicode substring");
 Check(wht::UnitNameReport131(L"A|rB")=="A/rB","native text cannot inject notification markup");
 wht::UnitNameSource131 target;target.selected.source.unit=1;target.selected.source.world=2;target.selected.source.agent=3;target.selected.source.id={4,5};target.selected.source.local=0;target.selected.info={0x68666f6f,10,20};target.name="|cffff0000Footman|r";
 unsigned reads=0,change=0;bool fail=false;
 auto read=[&](wht::UnitNameSource131& out){out=target;++reads;if(fail)return false;if(reads==2)switch(change){case 1:++out.selected.source.unit;break;case 2:++out.selected.source.world;break;case 3:++out.selected.source.agent;break;case 4:++out.selected.source.id.low;break;case 5:++out.selected.source.local;break;case 6:++out.selected.info.code;break;case 7:out.name="Knight";break;case 8:out.selected.info.x=NAN;break;case 9:out.selected.info.y=30;break;}return true;};
 auto run=[&](const std::wstring& args){reads=0;return wht::UnitNameCommand131(args,read,report);};
 Check(run(L"FOOT")&&reads==2&&report=="Found [Footman] in active selection at (10.0, 20.0)","confirmed actual native display name and coordinates");
 Check(!run(L"Knight")&&reads==2&&report.find("not found")!=report.npos,"arbitrary query cannot falsely report found");Check(!run(L"ffff0000"),"color digits not a name match");
 for(change=1;change<=8;++change)Check(!run(L"Foot"),"selection world registry name type or invalid position changed");change=9;Check(run(L"Foot")&&report.find("20.0, 30.0")==report.npos&&report.find("10.0, 30.0")!=report.npos,"moving same unit uses current position");change=0;
 Check(!run(L"bad\n")&&!reads,"invalid query no native reads");fail=true;Check(!run(L"Foot")&&reads==1,"unavailable selection fails truthfully");fail=false;target.name="";Check(!run(L"Foot"),"missing name no fabricated rawcode fallback");target.name=std::string("\xff",1);Check(!run(L"Foot"),"invalid cached encoding refused");
 puts("unit name contracts passed");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
'''
(r/'tests/unit-name-tests.cpp').write_text(s,encoding='utf-8')
