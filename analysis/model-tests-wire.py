from pathlib import Path
import json,hashlib
root=Path(__file__).resolve().parents[1]
src=(root/'tests/save-game-tests.cpp').read_text(encoding='utf-8');src=src[:src.index(' std::string name;')]
src=src.replace('save_game131.hpp','unit_model131.hpp').replace('saveGameProfiles131','unitModelProfiles131').replace('SaveGameCode131','UnitModelCode131')
src+=r'''
 for(const auto& p:wht::unitModelProfiles131){
  alignas(uintptr_t) unsigned char unit[128]{},model[256]{},name[128]{};char path[]="units\\human\\Footman\\Footman.mdx";const uintptr_t base=0x100000;
  auto put=[](unsigned char* dst,uintptr_t value){memcpy(dst,&value,sizeof(value));};
  put(unit+p.modelOffset,reinterpret_cast<uintptr_t>(model));put(model,base+p.modelTable);put(model+p.nameOffset,reinterpret_cast<uintptr_t>(name));put(name,base+p.stringTable);put(name+p.stringOffset,reinterpret_cast<uintptr_t>(path));
  uintptr_t m=0,n=0,t=0;char output[512]{};
  Check(wht::ReadUnitModelFields131(p,base,reinterpret_cast<uintptr_t>(unit),m,n,t,output)&&!strcmp(output,path),"exact model and STRINGREF chain");
  put(name,base+p.stringTable+8);Check(!wht::ReadUnitModelFields131(p,base,reinterpret_cast<uintptr_t>(unit),m,n,t,output),"adjacent/unrelated string class refused");put(name,base+p.stringTable);
  put(model,base+p.modelTable+8);Check(!wht::ReadUnitModelFields131(p,base,reinterpret_cast<uintptr_t>(unit),m,n,t,output),"unverified sprite class refused");put(model,base+p.modelTable);
  char longPath[512];memset(longPath,'a',sizeof(longPath));put(name+p.stringOffset,reinterpret_cast<uintptr_t>(longPath));Check(!wht::ReadUnitModelFields131(p,base,reinterpret_cast<uintptr_t>(unit),m,n,t,output),"unterminated filename refused");
 }
 Check(wht::UnitModelPath131("units\\hero.MDX")&&wht::UnitModelPath131("x.mdl"),"native asset suffixes");for(const auto* bad:{"", "units\\hfoo", "x.blp", "x|r.mdx", "x\n.mdx"})Check(!wht::UnitModelPath131(bad),"invalid/not-model notification path refused");
 wht::UnitModelSource131 value;value.owner.unit=1;value.owner.world=2;value.owner.agent=3;value.owner.local=0;value.owner.id={4,5};value.model=6;value.name=7;value.text=8;value.path="units\\Footman.mdx";unsigned reads=0;bool change=false;std::string report;
 auto read=[&](wht::UnitModelSource131& out){out=value;if(++reads==2&&change)out.path="units\\Other.mdx";return true;};
 Check(wht::UnitModelCommand131(L"",read,report)&&report=="Get Model: units\\Footman.mdx","confirmed native path only");reads=0;change=true;Check(!wht::UnitModelCommand131(L"",read,report),"changed selection/model filename refused");reads=0;Check(!wht::UnitModelCommand131(L" extra",read,report)&&!reads,"invalid args no game access");
 puts("unit model contracts passed (read-only, no fabricated rawcode filename)");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
'''
(root/'tests/unit-model-tests.cpp').write_text(src,encoding='utf-8')
p=root/'CMakeLists.txt';s=p.read_text(encoding='utf-8');s+='''
if(BUILD_TESTING)
 add_executable(WHT131UnitModelTests tests/unit-model-tests.cpp)
 target_link_libraries(WHT131UnitModelTests PRIVATE wht_common user32)
 target_compile_options(WHT131UnitModelTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME unit-model-contracts COMMAND WHT131UnitModelTests)
endif()
''';p.write_text(s,encoding='utf-8')
before={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (root/'runtime-test').rglob('*') if p.is_file() and p.suffix.lower() in ['.ini','.log'] and 'backup' not in str(p).lower()}
(root/'analysis/unit-model-preserved-before.json').write_text(json.dumps(before,indent=2),encoding='utf-8')
