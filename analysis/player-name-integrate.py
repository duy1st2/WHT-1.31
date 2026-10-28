from pathlib import Path
r=Path('WHT-1.31')
for arch in ['131','64']:
 p=r/f'native/version{arch}.cpp';s=p.read_text(encoding="utf-8-sig");anchor='#include "command_feedback131.hpp"';assert s.count(anchor)==1;s=s.replace(anchor,anchor+'\n#include "player_name131.hpp"')
 anchor=f'wht::SelfCastSource131 pinned{arch};';assert s.count(anchor)==1;s=s.replace(anchor,anchor+f'\nwht::PlayerNamePolicy131 playerName{arch};')
 marker=f'bool PinTarget{arch}()';pos=s.index(marker)
 helpers=f'''bool ReadName{arch}(wht::PlayerNameSource131& out){{return wht::ReadPlayerName131(base{arch},profile{arch}->build,out);}}
bool ApplyName{arch}(const wht::PlayerNameSource131& before,const std::string& name){{return wht::SetPlayerName131(base{arch},profile{arch}->build,before,name);}}
bool SaveName{arch}(const wchar_t* key,const std::wstring& name){{return wht::SaveSetting(ini{arch},L"Event",key,name);}}
'''
 s=s[:pos]+helpers+s[pos:]
 cmd='command' if arch=='131' else 'cmd';ui='currentUi131' if arch=='131' else 'ui64';anchor=f' if({cmd}==L"#cs")'
 pos=s.index(anchor)
 handler=f''' if({cmd}==L"#n"||{cmd}==L"#name"){{std::wstring args;std::getline(in,args);std::string report;const bool ok=wht::PlayerNameCommand131({cmd},args,config{arch},ReadName{arch},ApplyName{arch},SaveName{arch},Reload{arch},report);playerName{arch}.Reset();wht::Log(folder{arch},report);wht::ShowNotification131(base{arch},profile{arch}->build,{ui},report);return ok;}}
'''
 s=s[:pos]+handler+s[pos:]
 anchor=f'autoClick{arch}.Reset();pinned{arch}={{}};';assert s.count(anchor)==1;s=s.replace(anchor,anchor+f'playerName{arch}.Reset();')
 window='gameWindow131' if arch=='131' else 'window64';anchor=f'wht::UpdateWideScreen131(base{arch},profile{arch}->build,{window},Allowed{arch}()&&settings{arch},config{arch}.wide);'
 # One command invocation, one frame update. Insert only the final occurrence.
 pos=s.rindex(anchor)+len(anchor);s=s[:pos]+f'\n {{std::string report;playerName{arch}.Update(GetTickCount64(),Allowed{arch}()&&settings{arch},config{arch},ReadName{arch},ApplyName{arch},SaveName{arch},Reload{arch},report);if(!report.empty())wht::Log(folder{arch},report);}}\n'+s[pos:]
 p.write_text(s,encoding='utf-8')
p=r/'native/command_feedback131.hpp';s=p.read_text(encoding="utf-8-sig").replace('for(const auto* handled:{L"#item"','for(const auto* handled:{L"#n",L"#name",L"#item"');p.write_text(s,encoding='utf-8')
p=r/'native/player_name131.hpp';s=p.read_text(encoding="utf-8-sig").replace('settings unchanged','new name setting unchanged');p.write_text(s,encoding='utf-8')
p=r/'CMakeLists.txt';s=p.read_text(encoding="utf-8-sig");needle=' add_test(NAME create-copy-item-contracts COMMAND WHT131CreateItemTests)';assert s.count(needle)==1;s=s.replace(needle,needle+'''\n add_executable(WHT131PlayerNameTests tests/player-name-tests.cpp)
 target_link_libraries(WHT131PlayerNameTests PRIVATE wht_common user32)
 target_compile_options(WHT131PlayerNameTests PRIVATE /W4 /utf-8 /permissive-)
 add_test(NAME player-name-contracts COMMAND WHT131PlayerNameTests)''');p.write_text(s,encoding='utf-8')
print('Integrated commands and name policy on both architectures')
