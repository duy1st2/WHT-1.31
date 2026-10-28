from pathlib import Path
r=Path(__file__).resolve().parents[1]
for file,suf,cmd,ui,window,destroy in [('version131.cpp','131','command','currentUi131','gameWindow131','void DestroyUi131(){'),('version64.cpp','64','cmd','ui64','window64','void Destroy64(){')]:
 p=r/'native'/file;s=p.read_text(encoding='utf-8-sig');s=s.replace('#include "scroll131.hpp"','#include "scroll131.hpp"\n#include "wide_screen131.hpp"',1)
 marker=f' if({cmd}==L"#reloadconfig")';handler=f' if({cmd}==L"#wide"){{std::wstring args;std::getline(in,args);std::string report;const bool ok=wht::WideScreenSetting131(args,config{suf}.wide,Allowed{suf}()&&settings{suf},[&](){{return wht::InstallWideScreen131(base{suf},profile{suf}->build);}},[&](bool value){{return wht::SaveSetting(ini{suf},L"Event",L"WideScreen",value?L"1":L"0");}},[&](){{Reload{suf}();}},report);wht::UpdateWideScreen131(base{suf},profile{suf}->build,{window},Allowed{suf}()&&settings{suf},config{suf}.wide);wht::Log(folder{suf},report);wht::ShowNotification131(base{suf},profile{suf}->build,{ui},report);return ok;}}\n';assert marker in s;s=s.replace(marker,handler+marker,1)
 assert destroy in s;s=s.replace(destroy,destroy+'wht::ResetWideScreen131();',1)
 marker=f' if(!Allowed{suf}()||!settings{suf})' if suf=='131' else 'if(!Allowed64()||!settings64)'
 at=s.index(marker);s=s[:at]+f' wht::UpdateWideScreen131(base{suf},profile{suf}->build,{window},Allowed{suf}()&&settings{suf},config{suf}.wide);'+s[at:];p.write_text(s,encoding='utf-8-sig')
