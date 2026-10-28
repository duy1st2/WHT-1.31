from pathlib import Path
r=Path(__file__).resolve().parents[1]
for file in ['version131.cpp','version64.cpp']:
 p=r/'native'/file;s=p.read_text(encoding='utf-8-sig');s=s.replace('#include "create_unit131.hpp"','#include "create_unit131.hpp"\n#include "create_item131.hpp"',1)
 line=next(line for line in s.splitlines() if 'CreateOrCopyUnitCommand131' in line)
 new=line.replace('L"#create"','L"#item"').replace('L"#copy"','L"#copyitem"').replace('CreateOrCopyUnitCommand131','CreateOrCopyItemCommand131').replace('Create/Copy Unit:','Create/Copy Item:');s=s.replace(line,new+'\n'+line,1);p.write_text(s,encoding='utf-8-sig')
