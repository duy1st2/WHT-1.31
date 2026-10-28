from pathlib import Path
import hashlib,json,datetime,shutil
root=Path(__file__).resolve().parents[1]
refs=[root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp']
evidence={'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'behavior':'#chayga [1..3] maps chicken routes; #chaysoi/#caysoi [1..3] map wolf routes. Default1; parsed out-of-range integer falls back1. Existing MTH route engine, not a new native function. Reject malformed/extra/control input before worker access.'}
(root/'analysis/courier-aliases-reference.json').write_text(json.dumps(evidence,indent=2),encoding='utf-8')
backup=root/'analysis'/('before-courier-aliases-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'));backup.mkdir()
for rel in ['native/mth_route131.hpp','native/version131.cpp','native/version64.cpp','tests/mth-route131-contracts.hpp','README.md']:
 p=root/rel;shutil.copy2(p,backup/p.name)
p=root/'native/mth_route131.hpp';s=p.read_text(encoding='utf-8');at=s.index('struct MthRoute131 {');s=s[:at]+'''inline bool RouteAlias131(const std::wstring& name){return name==L"#chayga"||name==L"#chaysoi"||name==L"#caysoi";}
inline bool IsRouteCommand131(const std::wstring& name){return RouteCommand131(name)>=0||RouteAlias131(name);}
inline bool ParseRouteCommand131(const std::wstring& name,const std::wstring& args,int& route){
 route=-1;if(args.find(L'\\0')!=std::wstring::npos||args.find_first_of(L"\\r\\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring extra;const int canonical=RouteCommand131(name);
 if(canonical>=0){if(in>>extra)return false;route=canonical;return true;}
 if(!RouteAlias131(name))return false;
 int index=1;if(args.find_first_not_of(L" \\t")!=std::wstring::npos&&(!(in>>index)||in>>extra))return false;
 if(index<1||index>3)index=1;route=(name==L"#chayga"?0:3)+index-1;return true;
}
'''+s[at:];p.write_text(s,encoding='utf-8')
for rel,cmd,suffix,ui in [('native/version131.cpp','command','131','currentUi131'),('native/version64.cpp','cmd','64','ui64')]:
 p=root/rel;s=p.read_text(encoding='utf-8');old=f' if(const int route=wht::RouteCommand131({cmd});route>=0){{std::wstring extra;if(in>>extra)return false;'
 new=f' if(wht::IsRouteCommand131({cmd})){{std::wstring args;std::getline(in,args);int route=-1;if(!wht::ParseRouteCommand131({cmd},args,route)){{wht::ShowNotification131(base{suffix},profile{suffix}->build,{ui},"Route: #chayga [1..3], #chaysoi [1..3], #caysoi [1..3]");return false;}}'
 assert old in s;s=s.replace(old,new);p.write_text(s,encoding='utf-8')
p=root/'tests/mth-route131-contracts.hpp';s=p.read_text(encoding='utf-8');key='inline void CheckMthRoute131(){';s=s.replace(key,key+'''
 int parsed=-1;
 for(const auto* name:{L"#chayga",L"#chaysoi",L"#caysoi"}){
  const int offset=std::wstring(name)==L"#chayga"?0:3;
  check(wht::IsRouteCommand131(name)&&wht::ParseRouteCommand131(name,L"",parsed)&&parsed==offset,"legacy courier command default route1");
  for(int i=1;i<=3;++i)check(wht::ParseRouteCommand131(name,std::to_wstring(i),parsed)&&parsed==offset+i-1,"legacy aliases map each native route exactly");
  for(const auto* value:{L"0",L"-1",L"4"})check(wht::ParseRouteCommand131(name,value,parsed)&&parsed==offset,"reference out of range index defaults1");
  for(const auto* bad:{L"abc",L"1 extra",L"1x",L"2\\n",L"99999999999999999999"})check(!wht::ParseRouteCommand131(name,bad,parsed)&&parsed==-1,"invalid route alias cannot start a worker");
 }
 check(!wht::ParseRouteCommand131(L"#chayga",std::wstring(L"1\\0x",3),parsed),"embedded NUL refused");
 const wchar_t* canonical[]={L"#chicken1",L"#chicken2",L"#chicken3",L"#wolf1",L"#wolf2",L"#wolf3"};
 for(int i=0;i<6;++i)check(wht::ParseRouteCommand131(canonical[i],L"",parsed)&&parsed==i&&!wht::ParseRouteCommand131(canonical[i],L"1",parsed),"existing button/hotkey route grammar retained");
 check(!wht::IsRouteCommand131(L"#soi")&&!wht::ParseRouteCommand131(L"#other",L"",parsed),"unrelated existing command not intercepted");
''');p.write_text(s,encoding='utf-8')
before={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (root/'runtime-test').rglob('*') if p.is_file() and p.suffix.lower() in ['.ini','.log'] and 'backup' not in str(p).lower()}
(root/'analysis/courier-aliases-preserved-before.json').write_text(json.dumps(before,indent=2),encoding='utf-8')
print(backup)
