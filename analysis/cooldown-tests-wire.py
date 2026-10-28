from pathlib import Path
root=Path(__file__).resolve().parents[1]
src=(root/'tests/save-game-tests.cpp').read_text(encoding='utf-8');src=src[:src.index(' std::string name;')]
src=src.replace('save_game131.hpp','no_cooldown131.hpp').replace('saveGameProfiles131','cooldownResetProfiles131').replace('SaveGameCode131','CooldownResetCode131')
src+=r'''
 wht::NoCooldown131 state;std::string report;unsigned calls=0;bool ok=true;auto apply=[&](){++calls;return ok;};
 Check(state.Command(L"#nocd",L"",1000,apply,report)&&state.enabled&&calls==1&&state.due==1100,"enable invokes native once then schedules100ms");
 Check(!state.Tick(true,true,1099,apply)&&calls==1,"no early reset");Check(!state.Tick(true,true,1100,apply)&&calls==2&&state.due==1200,"100ms cadence");
 Check(!state.Tick(true,false,5000,apply)&&calls==2&&state.enabled,"menu/chat/focus pauses");Check(!state.Tick(true,true,5000,apply)&&calls==3&&state.due==5100,"resume once without catch up burst");
 Check(!state.Command(L"#nocd",L" extra",6000,apply,report)&&state.enabled&&calls==3,"bad args retain active state and invoke nothing");Check(!state.Command(L"#yescd",std::wstring(L"\0",1),6000,apply,report)&&state.enabled,"embedded NUL refused");
 Check(state.Command(L"#yescd",L"",6000,apply,report)&&!state.enabled&&calls==3,"disable never calls native");Check(!state.Tick(true,true,7000,apply)&&calls==3,"disabled no reset");
 ok=false;Check(!state.Command(L"#nocd",L"",8000,apply,report)&&!state.enabled&&calls==4,"failed initial reset never enables");ok=true;Check(state.Command(L"#nocd",L"",9000,apply,report),"re-enable");ok=false;Check(state.Tick(true,true,9100,apply)&&!state.enabled,"native failure stops and notifies once");const auto stoppedCalls=calls;Check(!state.Tick(true,true,9200,apply)&&calls==stoppedCalls,"failed reset not retried");
 ok=true;state.Command(L"#nocd",L"",10000,apply,report);const auto revokedCalls=calls;Check(!state.Tick(false,true,10100,apply)&&!state.enabled&&calls==revokedCalls,"authorization loss resets without native call");
 auto reentrant=[&](){state.Reset();return true;};Check(!state.Command(L"#nocd",L"",11000,reentrant,report)&&!state.enabled,"callback teardown cannot re-enable stale command");
 wht::RemoveUnitSource131 source;source.unit=1;source.world=2;source.agent=3;source.local=0;source.id={4,5};unsigned reads=0,writes=0,validations=0;bool changed=false,valid=true;
 auto read=[&](wht::RemoveUnitSource131& out){out=source;if(++reads==2&&changed)++out.id.high;return true;};auto reset=[&](const wht::RemoveUnitSource131& value){++writes;return value.unit==source.unit;};auto verify=[&](const wht::RemoveUnitSource131&){++validations;return valid;};
 Check(wht::ResetCooldownFlow131(read,reset,verify)&&writes==1&&validations==2,"stable registry checked before and after native");changed=true;reads=writes=validations=0;Check(!wht::ResetCooldownFlow131(read,reset,verify)&&!writes&&!validations,"changed selection identity before reset refused");changed=false;valid=false;reads=writes=validations=0;Check(!wht::ResetCooldownFlow131(read,reset,verify)&&!writes,"invalid registry no native call");
 Check(!wht::InvokeCooldownReset131(nullptr,1)&&!wht::InvokeCooldownReset131(nullptr,0),"null native refused");
 puts("no cooldown contracts passed (no live cooldown changed)");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
'''
(root/'tests/no-cooldown-tests.cpp').write_text(src,encoding='utf-8')
p=root/'CMakeLists.txt';s=p.read_text(encoding='utf-8');s+='''
if(BUILD_TESTING)
 add_executable(WHT131NoCooldownTests tests/no-cooldown-tests.cpp)
 target_link_libraries(WHT131NoCooldownTests PRIVATE wht_common user32)
 target_compile_options(WHT131NoCooldownTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME no-cooldown-contracts COMMAND WHT131NoCooldownTests)
endif()
''';p.write_text(s,encoding='utf-8')
