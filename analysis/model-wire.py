from pathlib import Path
import shutil,datetime
root=Path(__file__).resolve().parents[1];backup=root/'analysis'/('before-unit-model-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for name in ['native/version131.cpp','native/version64.cpp','CMakeLists.txt','README.md']:
 p=root/name;shutil.copy2(p,backup/p.name)
 if 'version' not in name:continue
 text=p.read_text(encoding='utf-8');text=text.replace('#include "save_game131.hpp"','#include "save_game131.hpp"\n#include "unit_model131.hpp"')
 x64='64' in p.stem;cmd='cmd' if x64 else 'command';suffix='64' if x64 else '131';ui='ui64' if x64 else 'currentUi131';selected='wht::Selected64(ui64,base64,*profile64)' if x64 else 'SelectedUnit131()'
 # Use the same selected-unit accessor as existing native unit commands.
 if not x64:
  import re
  selected=re.search(r'ReadUnitScale131\(base131,profile131->build,\[&\]\(\)\{return (.*?);\}',text).group(1)
 line=f' if({cmd}==L"#gmodel"){{std::wstring args;std::getline(in,args);std::string report;const bool ok=settings{suffix}&&wht::UnitModelCommand131(args,[&](wht::UnitModelSource131& out){{return wht::ReadUnitModel131(base{suffix},profile{suffix}->build,[&](){{return {selected};}},out);}},report);if(report.empty())report="Get Model: settings disabled";wht::Log(folder{suffix},report);wht::ShowNotification131(base{suffix},profile{suffix}->build,{ui},report);return ok;}}\n'
 at=text.index(f' if({cmd}==L"#emodel"');text=text[:at]+line+text[at:];p.write_text(text,encoding='utf-8')
print(backup)
