#include "../native/adaptive_effects131.hpp"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdio>
void Check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
unsigned sets=0;float nativeValue=1;
bool __cdecl Setter(float v){++sets;nativeValue=v;return true;}
struct Image {
 uintptr_t base=0;
 Image(const char* file,const wht::EffectsProfile131& p){std::ifstream in(file,std::ios::binary);Check(bool(in),"captured image exists");std::vector<unsigned char> b((std::istreambuf_iterator<char>(in)),{});base=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,b.size(),MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));Check(base!=0,"owned image");memcpy(reinterpret_cast<void*>(base),b.data(),b.size());const auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b.data());const auto nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(b.data()+dos->e_lfanew);const auto capturedBase=uintptr_t(nt->OptionalHeader.ImageBase);
 for(size_t i=0;i<p.count;++i){const auto& c=p.code[i];std::vector<unsigned char> expected(c.bytes,c.bytes+c.length);for(size_t j=0;j<c.relocCount;++j)*reinterpret_cast<unsigned*>(expected.data()+c.reloc[j])+=unsigned(capturedBase);Check(!memcmp(b.data()+c.rva,expected.data(),c.length),"guard bytes match independent captured engine before rebasing");memcpy(reinterpret_cast<void*>(base+c.rva),c.bytes,c.length);for(size_t j=0;j<c.relocCount;++j)*reinterpret_cast<unsigned*>(base+c.rva+c.reloc[j])+=unsigned(base);}
 *reinterpret_cast<uintptr_t*>(base+p.table)=base+p.method;*reinterpret_cast<uintptr_t*>(base+p.table-sizeof(uintptr_t))=base+p.locator;
#ifndef _WIN64
 *reinterpret_cast<uintptr_t*>(base+p.locator+12)=base+p.type;
#endif
 }
 ~Image(){if(base)VirtualFree(reinterpret_cast<void*>(base),0,MEM_RELEASE);}
};
int main(){try{
 Check(!wht::EffectsProfileFor131(129),"unknown build refused");
 for(unsigned build:{12164u,12173u}){const auto* p=wht::EffectsProfileFor131(build);Check(p!=nullptr,"independent build profile");
#ifdef _WIN64
 const char* path=build==12164?"../analysis/memory-24984.bin":"../analysis/memory-27336.bin";
#else
 const char* path=build==12164?"../analysis/memory-8740.bin":"../analysis/memory-32116.bin";
#endif
 Image image(path,*p);Check(wht::EffectsCode131(image.base,*p)&&!wht::EffectsCode131(0,*p),"rebased full native and RTTI guards");
 for(size_t i=0;i<p->count;++i){auto& byte=*reinterpret_cast<unsigned char*>(image.base+p->code[i].rva);byte^=1;Check(!wht::EffectsCode131(image.base,*p),"modified native code rejected");byte^=1;}
 auto& type=*reinterpret_cast<char*>(image.base+p->type+2*sizeof(uintptr_t));type='!';Check(!wht::EffectsCode131(image.base,*p),"wrong particle type refused");type='.';
 std::array<unsigned char,0x100> renderer{};*reinterpret_cast<uintptr_t*>(image.base+p->renderer)=reinterpret_cast<uintptr_t>(renderer.data());*reinterpret_cast<uint32_t*>(renderer.data()+(sizeof(uintptr_t)==8?0x84:0x50))=5999;*reinterpret_cast<float*>(image.base+p->scale)=.8f;float fps=0,scale=0;Check(wht::EffectsRead131(image.base,build,fps,scale)&&fps>59.98f&&fps<60&&scale==.8f,"native stock counter7 divided100, independent of show FPS");*reinterpret_cast<float*>(image.base+p->scale)=2;Check(!wht::EffectsRead131(image.base,build,fps,scale),"invalid scale refused");
 }
 Check(wht::InvokeEffectsSetter131(Setter,.5f)&&nativeValue==.5f&&sets==1,"cdecl float argument and bool AL result");Check(!wht::InvokeEffectsSetter131(Setter,-1)&&!wht::InvokeEffectsSetter131(Setter,2)&&sets==1,"invalid args no setter call");
 wht::EffectsController131 c;float fps=40,scale=.8f;unsigned reads=0,writes=0;bool rejected=false;std::string report;
 auto read=[&](float& f,float& s){++reads;f=fps;s=scale;return true;};auto set=[&](float v){++writes;if(rejected)return false;scale=v;return true;};auto tick=[&](uint64_t now,bool enabled=true,bool foreground=true,uintptr_t world=123){return c.Tick(now,world,enabled,foreground,read,set,report);};
 Check(tick(1000)&&!writes&&scale==.8f,"low FPS starts observation without immediate reduction");const auto prior=reads;Check(tick(1100)&&reads==prior,"250 ms sampling limit");Check(tick(2999)&&!writes&&tick(3250)&&scale==.4f,"sustained low FPS half emission");Check(tick(5250)&&scale==.2f,"second low-FPS step quarter emission");fps=10;Check(tick(5500)&&tick(6000)&&scale==0,"critical FPS zero new emission after500 ms");fps=60;Check(tick(6250)&&tick(14250)&&scale==.2f&&tick(22250)&&scale==.4f&&tick(30250)&&scale==.8f,"slow one-step recovery at58 FPS after8 s");
 fps=10;Check(tick(30500)&&tick(31000)&&scale==0,"critical reduction repeat");Check(tick(31250,false)&&scale==.8f&&!c.state.owned,"disabled restores captured original");
 c={};scale=.8f;fps=40;Check(tick(32000)&&tick(34000)&&scale==.4f,"re-enable");scale=.6f;Check(tick(34250,false)&&scale==.6f,"external video changes preserved");
 c={};scale=.8f;fps=40;Check(tick(35000)&&tick(37000)&&scale==.4f,"session fixture");Check(tick(37250,true,true,456)&&scale==.8f&&!c.state.owned,"world change restores before new session samples");
 c={};scale=.8f;fps=40;Check(tick(38000)&&tick(40000,true,false)&&tick(41000)&&!c.state.owned&&scale==.8f,"background gap does not count toward low FPS dwell");rejected=true;Check(!tick(43000)&&scale==.8f&&!c.state.owned,"setter refusal preserves original scale");rejected=false;
 c={};scale=.8f;fps=10;Check(tick(50000)&&tick(50500)&&c.state.owned&&scale==0,"restore-failure fixture");rejected=true;report.clear();Check(!tick(50750,false)&&c.state.owned&&!report.empty()&&scale==0,"failed restore retains ownership for retry");report.clear();Check(!tick(51000,false)&&report.empty(),"persistent failure logged once");rejected=false;Check(tick(51250,false)&&!c.state.owned&&scale==.8f,"restore retries successfully");
 wht::Config config;Check(!config.autoEffects,"opt-in default");unsigned saves=0,reloads=0;bool saved=false,canSave=true;auto save=[&](bool v){++saves;if(!canSave)return false;saved=v;return true;};auto reload=[&](){++reloads;config.autoEffects=saved;};
 Check(wht::EffectsCommand131(L"1",config,[](){return true;},save,reload,report)&&config.autoEffects,"command saves/reloads enable");Check(wht::EffectsCommand131(L"",config,[](){return true;},save,reload,report)&&!config.autoEffects,"bare command toggles");const auto before=saves;Check(!wht::EffectsCommand131(L"1 extra",config,[](){return true;},save,reload,report)&&!wht::EffectsCommand131(L"1",config,[](){return false;},save,reload,report)&&saves==before,"invalid input and unverified native no save");canSave=false;const auto reloadBefore=reloads;Check(!wht::EffectsCommand131(L"1",config,[](){return true;},save,reload,report)&&reloads==reloadBefore&&!config.autoEffects,"save failure no reload");
 puts("Adaptive particle profiles, FPS, ABI, policy, lifecycle and command contracts passed");return 0;
 }catch(const std::exception& e){puts(e.what());return 1;}}
