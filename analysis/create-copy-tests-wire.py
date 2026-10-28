from pathlib import Path
root=Path(__file__).resolve().parents[1]
src=(root/'tests/save-game-tests.cpp').read_text(encoding='utf-8');src=src[:src.index(' std::string name;')]
src=src.replace('save_game131.hpp','create_unit131.hpp').replace('saveGameProfiles131','createUnitProfiles131').replace('SaveGameCode131','CreateUnitCode131')
src=src.replace(' Check(wht::CreateUnitCode131(base,p)', ''' const auto* rp=wht::ResourceProfileFor131(p.build);const auto slot=rp->playerTable+(sizeof(uintptr_t)==8?0xc8:0x64);uintptr_t nativeGetter=0;input.seekg(slot);input.read(reinterpret_cast<char*>(&nativeGetter),sizeof(nativeGetter));Check(nativeGetter==capturedBase+p.ownerGetter&&p.ownerGetter==rp->idGetter,"native player virtual owner getter matches captured table and resource identity");nativeGetter=base+p.ownerGetter;memcpy(image.data+slot,&nativeGetter,sizeof(nativeGetter));
 float facing=0;Check(wht::CreateFacing131(base,p,facing)&&fabsf(facing-4.712389f)<.000001f,"native degree-to-radian scalar verified");
 Check(wht::CreateUnitCode131(base,p)''')
definitions=r'''
unsigned nativeOwner=0;uint32_t nativeCode=0;float nativeX=0,nativeY=0,nativeFacing=0;
uintptr_t __cdecl MockCreateUnit(unsigned owner,uint32_t code,const float* x,const float* y,const float* facing){nativeOwner=owner;nativeCode=code;nativeX=*x;nativeY=*y;nativeFacing=*facing;return 0x123456;}
'''
src=src.replace('int main(){try{',definitions+'\nint main(){try{')
src+=r'''
 uint32_t code=0;unsigned count=0;Check(wht::ParseCreateUnit131(L"hfoo",code,count)&&code==0x68666f6f&&count==1,"default fresh unit count");Check(wht::ParseCreateUnit131(L"0x68666F6F 50",code,count)&&count==50,"hex rawcode maximum50");Check(wht::ParseUnitCount131(L"",count)&&count==1&&wht::ParseUnitCount131(L" 3 ",count)&&count==3,"copy count/default");for(const auto* bad:{L"0",L"51",L"-1",L"+1",L"one",L"1 2",L"1x",L"2\n"})Check(!wht::ParseUnitCount131(bad,count),"malformed count refused");Check(!wht::ParseCreateUnit131(std::wstring(L"hfoo\0 1",8),code,count)&&!wht::ParseCreateUnit131(L"hfoo 1 extra",code,count),"NUL/extra args refused");
 Check(wht::InvokeCreateUnit131(MockCreateUnit,2,0x68666f6f,100,200,4.712389f)==0x123456&&nativeOwner==2&&nativeCode==0x68666f6f&&nativeX==100&&nativeY==200&&nativeFacing==4.712389f,"cdecl worker ABI physical owner rawcode and three real pointers");Check(!wht::InvokeCreateUnit131(nullptr,0,1,0,0,0)&&!wht::InvokeCreateUnit131(MockCreateUnit,24,1,0,0,0),"invalid native/owner no call");
 wht::ResourceCommandSource131 context;context.world=1;context.player=2;context.local=0;context.physical=2;unsigned reads=0,creates=0,confirms=0,changedAt=0,failAt=0;bool valid=true,confirmOK=true,duplicate=false;std::string report;
 auto read=[&](wht::ResourceCommandSource131& out){out=context;if(++reads==changedAt)++out.world;return true;};auto sourceValid=[&](const auto&){return valid;};
 auto create=[&](unsigned owner,uint32_t type,float x,float y)->uintptr_t{Check(owner==2&&type==0x68666f6f&&x==100+float(creates)*40&&y==200,"physical local player and frozen spacing");++creates;return creates==failAt?0:creates;};
 auto confirm=[&](uintptr_t unit,uint32_t type,const auto& expected,wht::CreatedUnit131& out){++confirms;Check(unit==creates&&type==0x68666f6f&&expected.world==context.world,"confirm exact returned native unit");out.source.id={duplicate?1u:unsigned(unit),5};return confirmOK;};
 auto run=[&](unsigned n=3,float x=100){reads=creates=confirms=0;return wht::CreateUnitBatch131(0x68666f6f,n,x,200,40,read,sourceValid,create,confirm,report);};
 Check(run()&&creates==3&&confirms==3&&report=="Create Unit: 3/3 created","confirmed bounded batch");changedAt=3;Check(!run()&&!creates,"world changed before native no spawn");changedAt=4;Check(!run()&&creates==1&&report.find("latest invocation")!=report.npos,"world changed during spawn stops without retry");changedAt=0;failAt=2;Check(!run()&&creates==2&&confirms==1&&report.find("1/3")!=report.npos,"partial batch reports only verified creations");failAt=0;confirmOK=false;Check(!run()&&creates==1,"wrong/unregistered returned unit stops");confirmOK=true;duplicate=true;Check(!run()&&creates==2&&report.find("1/3")!=report.npos,"duplicate registry identity cannot count twice");duplicate=false;valid=false;Check(!run()&&!creates,"changed copy source before batch no native write");valid=true;Check(!run(51)&&!reads&&!creates&&!run(2,99990)&&!reads&&!creates,"count and complete coordinate extent validated before game reads");
 unsigned accessed=0;auto selected=[&]()->uintptr_t{++accessed;return 0;};auto cursor=[&](wht::MouseWorldPosition131&){++accessed;return false;};Check(!wht::CreateOrCopyUnitCommand131(0,12164,L"#create",L"hfoo 0",selected,cursor,report)&&!accessed,"bad args no game access");Check(!wht::CreateOrCopyUnitCommand131(0,12164,L"#copy",L"1",selected,cursor,report)&&!accessed,"profile mismatch no game source reads");Check(!wht::createUnitBusy131.test_and_set(),"failed command releases batch guard");wht::createUnitBusy131.clear();
 puts("create/copy unit contracts passed (owned mock worker only, no live unit created)");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
'''
(root/'tests/create-unit-tests.cpp').write_text(src,encoding='utf-8')
p=root/'CMakeLists.txt';s=p.read_text(encoding='utf-8');s+='''
if(BUILD_TESTING)
 add_executable(WHT131CreateUnitTests tests/create-unit-tests.cpp)
 target_link_libraries(WHT131CreateUnitTests PRIVATE wht_common user32)
 target_compile_options(WHT131CreateUnitTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME create-copy-unit-contracts COMMAND WHT131CreateUnitTests)
endif()
''';p.write_text(s,encoding='utf-8')
