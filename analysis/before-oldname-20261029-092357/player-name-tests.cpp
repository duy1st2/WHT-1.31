#include "../native/player_name131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool b,const char* s){if(!b)throw std::runtime_error(s);}
uintptr_t seenField=0;std::string seenName;
#ifdef _WIN64
void NativeSet(uintptr_t field,const char* value){seenField=field;seenName=value;}
#else
void __fastcall NativeSet(uintptr_t field,uintptr_t,const char* value){seenField=field;seenName=value;}
#endif
struct Model {
 wht::Config config;wht::PlayerNameSource131 source{1,2,0,"Alpha"};unsigned writes=0,saves=0,reads=0;bool available=true,failSet=false,changeWorld=false,wrongName=false;std::wstring failKey;
 bool Read(wht::PlayerNameSource131& out){++reads;if(!available)return false;out=source;return true;}
 bool Set(const wht::PlayerNameSource131& old,const std::string& desired){++writes;if(failSet)return false;Check(wht::SamePlayerNameContext131(old,source),"same source before setter");strcpy_s(source.name,wrongName?"Other":desired.c_str());if(changeWorld)++source.world;return true;}
 bool Save(const wchar_t* key,const std::wstring& value){++saves;if(failKey==key)return false;const std::wstring k=key;if(k==L"OLDNames")config.oldNames=value;else if(k==L"OLDNames_Ex")config.oldNamesEx=value;else if(k==L"NewNames")config.newNames=value;else if(k==L"NewNames_Ex")config.newNamesEx=value;else Check(false,"unexpected key");return true;}
 bool Command(const std::wstring& args,std::string& report,const std::wstring& name=L"#n"){return wht::PlayerNameCommand131(name,args,config,[&](auto& out){return Read(out);},[&](const auto& old,const auto& desired){return Set(old,desired);},[&](auto key,const auto& value){return Save(key,value);},[](){},report);}
 void Tick(wht::PlayerNamePolicy131& policy,uint64_t now,std::string& report,bool allowed=true){report.clear();policy.Update(now,allowed,config,[&](auto& out){return Read(out);},[&](const auto& old,const auto& desired){return Set(old,desired);},[&](auto key,const auto& value){return Save(key,value);},[](){},report);}
};
int main(){try{
 wht::PlayerNameRequest131 request;std::string utf8,report;
 Check(wht::ParsePlayerName131(L"  ",request)&&request.query,"query");Check(wht::ParsePlayerName131(L"  New Name  ",request)&&request.text==L"New Name"&&!request.restore,"trim name");
 for(auto s:{L"restore",L"default",L"reset",L"off"})Check(wht::ParsePlayerName131(s,request)&&request.restore,"restore aliases");
 Check(wht::PlayerNameText131(std::wstring(63,L'a'),utf8)&&utf8.size()==63,"63 ASCII bytes");Check(!wht::PlayerNameText131(std::wstring(64,L'a'),utf8),"64 bytes refused");Check(wht::PlayerNameText131(std::wstring(21,L'\u1ebf'),utf8)&&utf8.size()==63,"63 UTF8 bytes intact");Check(!wht::PlayerNameText131(std::wstring(22,L'\u1ebf'),utf8),"UTF8 size bound");Check(!wht::PlayerNameText131(std::wstring(1,wchar_t(0xd800)),utf8),"unpaired surrogate");
 for(auto s:{L"a\nb",L"a\rb",L"a\tb",L"a\x7f"})Check(!wht::ParsePlayerName131(s,request),"control character rejected");Check(!wht::ParsePlayerName131(std::wstring(L"a\0b",3),request),"nul rejected");
 const uintptr_t field=sizeof(void*)==8?uintptr_t(0x1234567887654321ULL):uintptr_t(0x12345678);Check(wht::InvokePlayerName131(reinterpret_cast<wht::PlayerNameSetter131>(NativeSet),field,"Test")&&seenField==field&&seenName=="Test","native thiscall/Win64 ABI pointer and argument");Check(!wht::InvokePlayerName131(nullptr,field,"Test"),"missing native");
 Model m;Check(m.Command(L"",report)&&report=="Player Name: Alpha"&&!m.writes&&!m.saves,"query read only");Check(m.Command(L"Beta",report)&&m.config.oldNames==L"Alpha"&&m.config.oldNamesEx==L"Alpha"&&m.config.newNames==L"Beta"&&m.config.newNamesEx==L"Beta"&&!strcmp(m.source.name,"Beta"),"native rename and original preserved");Check(m.Command(L"Gamma",report)&&m.config.oldNames==L"Alpha","later rename retains original");Check(m.Command(L"restore",report)&&!strcmp(m.source.name,"Alpha")&&m.config.newNames.empty()&&m.config.newNamesEx.empty(),"restore clears pending rename");Check(m.Command(L"",report,L"#name")&&report=="[64] Player Name: Alpha","reference alias label");
 Model absent;Check(!absent.Command(L"restore",report)&&!absent.writes&&!absent.saves,"no original no write");absent.available=false;Check(!absent.Command(L"New",report)&&!absent.writes&&!absent.saves,"no context no mutation");
 Model failed;failed.failKey=L"OLDNames";Check(!failed.Command(L"New",report)&&!failed.writes,"cannot preserve original no native call");
 Model nativeFail;nativeFail.failSet=true;Check(!nativeFail.Command(L"New",report)&&nativeFail.config.newNames.empty(),"native failure no persisted desired name");
 for(bool context:{false,true}){Model race;race.changeWorld=context;race.wrongName=!context;Check(!race.Command(L"New",report)&&race.config.newNames.empty(),"wrong readback/context no success or desired save");}
 Model saveFail;saveFail.failKey=L"NewNames_Ex";Check(!saveFail.Command(L"New",report)&&report.find("settings not fully saved")!=report.npos&&!strcmp(saveFail.source.name,"New"),"partial persistence reported honestly");
 wht::PlayerNamePolicy131 policy;Model automatic;automatic.config.newNames=L"Auto";automatic.config.newNamesEx=L"Auto";automatic.Tick(policy,1,report);Check(automatic.writes==1&&automatic.config.oldNames==L"Alpha","configured name applied and original captured");strcpy_s(automatic.source.name,"Map changed");automatic.Tick(policy,500,report);Check(automatic.writes==1,"one second cadence");automatic.Tick(policy,1001,report);Check(automatic.writes==2&&!strcmp(automatic.source.name,"Auto"),"configured name reapplied");automatic.Tick(policy,2001,report,false);Check(!policy.world,"disabled resets policy");
 policy.Reset();Model quarantine;quarantine.config.newNames=L"Auto";quarantine.failSet=true;quarantine.Tick(policy,1,report);quarantine.Tick(policy,1001,report);Check(quarantine.writes==1&&policy.blocked,"failed context quarantined");++quarantine.source.world;quarantine.Tick(policy,2001,report);Check(quarantine.writes==2,"new world may retry");quarantine.config.newNames=L"Other";quarantine.Tick(policy,3001,report);Check(quarantine.writes==3,"new configured name may retry");
 for(const auto& p:wht::playerNameProfiles131){Check(!wht::PlayerNameCode131(0,p),"null guard");
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream in(file,std::ios::binary);Check(bool(in),"capture");std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});const auto base=reinterpret_cast<uintptr_t>(image.data());for(size_t i=0;i<p.count;++i){const auto& c=p.code[i];for(size_t j=0;j<c.relocCount;++j){unsigned value=0;memcpy(&value,c.bytes+c.reloc[j],4);value+=unsigned(base);memcpy(image.data()+c.rva+c.reloc[j],&value,4);}}Check(wht::PlayerNameCode131(base,p),"rebased native body guards");for(size_t i=0;i<p.count;++i){image[p.code[i].rva]^=1;Check(!wht::PlayerNameCode131(base,p),"modified setter/getter refuses");image[p.code[i].rva]^=1;}
 }
 puts("Player name contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
