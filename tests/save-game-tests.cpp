#include "../native/save_game131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
struct Fixture {unsigned char* data=nullptr;explicit Fixture(size_t size){data=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));Check(data!=nullptr,"private fixture allocation");}~Fixture(){if(data)VirtualFree(data,0,MEM_RELEASE);}};
int main(){try{
 for(const auto& p:wht::saveGameProfiles131){
#ifdef _WIN64
 const char* file=p.build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* file=p.build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 std::ifstream input(file,std::ios::binary);Check(bool(input),"capture fixture available");unsigned char header[4096]{};input.read(reinterpret_cast<char*>(header),sizeof(header));unsigned pe=0;memcpy(&pe,header+60,4);uintptr_t capturedBase=0;memcpy(&capturedBase,header+pe+24+(sizeof(uintptr_t)==8?24:28),sizeof(uintptr_t));
 size_t size=0;for(size_t i=0;i<p.count;++i)size=(std::max)(size,size_t(p.code[i].rva+p.code[i].length));Fixture image(size+64);const auto base=reinterpret_cast<uintptr_t>(image.data);
 for(size_t i=0;i<p.count;++i){const auto& block=p.code[i];std::vector<unsigned char> bytes(block.length);input.seekg(block.rva);input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());Check(size_t(input.gcount())==bytes.size(),"full captured block read");
 for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,bytes.data()+block.reloc[j],4);value-=uint32_t(capturedBase);memcpy(bytes.data()+block.reloc[j],&value,4);}Check(!memcmp(bytes.data(),block.bytes,block.length),"full captured wrapper initializer step and table match");
 memcpy(image.data+block.rva,bytes.data(),bytes.size());for(size_t j=0;j<block.relocCount;++j){uint32_t value=0;memcpy(&value,image.data+block.rva+block.reloc[j],4);value+=uint32_t(base);memcpy(image.data+block.rva+block.reloc[j],&value,4);}}
 Check(wht::SaveGameCode131(base,p),"relocated full profile guard");for(size_t i=0;i<p.count;++i){image.data[p.code[i].rva]^=1;Check(!wht::SaveGameCode131(base,p),"tampered native block/table refused");image.data[p.code[i].rva]^=1;}


 }

 std::string name;Check(wht::ParseSaveName131(L"",name)&&name=="wht-save","default save name");Check(wht::ParseSaveName131(L" My-Save_01 ",name)&&name=="My-Save_01","named save");Check(wht::ParseSaveName131(std::wstring(64,L'a'),name),"64 chars accepted");for(const auto* bad:{L"CON",L"nul",L"COM1",L"LPT9",L"../escape",L"E:\other",L"a/b",L"a.b",L"two words",L"abc\n",L"|r"})Check(!wht::ParseSaveName131(bad,name),"invalid path/name rejected");Check(!wht::ParseSaveName131(std::wstring(65,L'a'),name)&&!wht::ParseSaveName131(std::wstring(L"abc\0x",5),name),"long/NUL rejected");
 std::wstring path;const std::wstring arch=sizeof(uintptr_t)==8?L"x86_64":L"x86";const std::wstring exe=LR"(E:\Game\WHT-1.31\runtime-test\Warcraft3-1.31.1.12164\)"+arch+LR"(\Warcraft III.exe)";
 Check(wht::SaveDestination131(exe,12164,"test",path)&&path==LR"(E:\Game\WHT-1.31\runtime-test\Warcraft3-1.31.1.12164\WHT-Saves\test.w3z)","output beneath client runtime-test with no user path");Check(!wht::SaveDestination131(exe,12173,"test",path),"build placement mismatch rejected");Check(!wht::SaveDestination131(LR"(E:\Warcraft III\)"+arch+LR"(\Warcraft III.exe)",12164,"test",path),"original game placement refused");Check(!wht::SaveDestination131(LR"(E:\Bad\..\Game\WHT-1.31\runtime-test\Warcraft3-1.31.1.12164\)"+arch+LR"(\Warcraft III.exe)",12164,"test",path),"noncanonical path refused");Check(!wht::SaveDestination131(exe,12164,"../bad",path)&&!wht::SaveDestination131(exe,12164,"",path),"invalid basename cannot escape destination");Check(!wht::SaveDestination131(LR"(E:\)"+std::wstring(200,L'a')+exe.substr(2),12164,"test",path),"native filename maximum enforced");
 wht::SaveSource131 source;source.player.world=123;source.player.player=456;source.player.local=source.player.physical=0;source.session=789;unsigned reads=0,prepares=0,saves=0,confirms=0,changedAt=0;bool prepareOK=true,saveOK=true,confirmOK=true;std::string report;
 auto read=[&](wht::SaveSource131& out){out=source;if(++reads==changedAt)++out.session;return true;};auto prepare=[&](const std::string& n,std::wstring& out){++prepares;Check(n=="wht-save","default basename only");out=L"owned-path";return prepareOK;};auto save=[&](const std::wstring&){++saves;return saveOK;};auto confirm=[&](const std::wstring&){++confirms;return confirmOK;};auto run=[&](const std::wstring& args){reads=prepares=saves=confirms=0;return wht::SaveGameFlow131(args,read,prepare,save,confirm,report);};
 Check(run(L"")&&saves==1&&confirms==1&&report.find("Saved:")!=report.npos,"success needs native bool and file confirmation");Check(!run(L"../escape")&&!reads&&!prepares&&!saves,"bad input no game/filesystem access");prepareOK=false;Check(!run(L"")&&!saves,"existing file/outside path/link refused before native write");prepareOK=true;changedAt=3;Check(!run(L"")&&!saves,"match changed after path preparation");changedAt=4;Check(!run(L"")&&saves==1&&!confirms,"match changed during native save no success claim");changedAt=0;saveOK=false;Check(!run(L"")&&saves==1&&!confirms,"native failure no retry");saveOK=true;confirmOK=false;Check(!run(L"")&&report.find("not confirmed")!=report.npos,"missing/empty output not success");confirmOK=true;source.session=0;Check(!run(L"")&&!prepares&&!saves,"no active session no writes");
 Check(!wht::InvokeSaveWorker131(nullptr,"x"),"null native refused");puts("save game contracts passed (no live game save invoked)");return 0;}catch(const std::exception& e){puts(e.what());return 1;}}
