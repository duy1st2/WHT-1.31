#pragma once
#include "mpq_loader131.hpp"
#include <filesystem>
namespace wht {
inline bool IsFastJassDll131(const std::filesystem::path& path){const auto name=DllIdentity131(path.filename().wstring());return DllIdentity131(path.extension().wstring())==L".dll"&&name.find(L"fastjass")!=name.npos;}
inline bool ParseFastJass131(const std::wstring& args,bool current,bool& wanted){
 if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;std::wistringstream in(args);std::wstring value,extra;if(!(in>>value)){wanted=!current;return true;}if(in>>extra)return false;
 value=DllIdentity131(value);if(value==L"1"||value==L"on"||value==L"true")wanted=true;else if(value==L"0"||value==L"off"||value==L"false")wanted=false;else return false;return true;
}
struct LibraryScan131 {unsigned loaded=0,mounted=0,already=0,skippedFast=0,failed=0;bool directoryError=false;bool Ok()const{return !failed&&!directoryError;}};
struct WhtLibrary131 {
 bool started=false,dllStarted=false,scanning=false;unsigned archivePriority=30;
 template<class Mount,class Log> LibraryScan131 Scan(const std::wstring& folder,bool fast,DllLoader131& dll,MpqLoader131& mpq,Mount mount,Log log,bool archives=true,bool dlls=true){
  LibraryScan131 result;if(scanning){result.failed=1;log("WHT_Lib: scan already in progress");return result;}scanning=true;struct Unlock{bool& flag;~Unlock(){flag=false;}} unlock{scanning};
  std::vector<std::filesystem::path> paths;const auto root=std::filesystem::path(folder),lib=root/L"WHT_Lib";std::error_code ec;
  const bool exists=std::filesystem::exists(lib,ec);if(ec){result.directoryError=true;log("WHT_Lib: cannot inspect directory");}
  if(exists){std::filesystem::directory_iterator it(lib,ec),end;if(ec){result.directoryError=true;log("WHT_Lib: cannot open directory");}
   while(!ec&&it!=end){std::error_code fileError;if(it->is_regular_file(fileError)){const auto ext=DllIdentity131(it->path().extension().wstring());if((dlls&&ext==L".dll")||(archives&&(ext==L".mpq"||ext==L".mix")))paths.push_back(it->path());}if(fileError){result.directoryError=true;log("WHT_Lib: cannot inspect file");}it.increment(ec);}
   if(ec){result.directoryError=true;log("WHT_Lib: directory scan incomplete");}
  }
  std::sort(paths.begin(),paths.end(),[](const auto& a,const auto& b){return DllIdentity131(a.wstring())<DllIdentity131(b.wstring());});
  if(fast&&dlls){const auto fj=root/L"FastJass.dll";std::error_code fileError;if(std::filesystem::is_regular_file(fj,fileError))paths.push_back(fj);else if(fileError&&fileError!=std::errc::no_such_file_or_directory){result.directoryError=true;log("FastJass: cannot inspect root DLL");}}
  for(const auto& path:paths){const auto requested=path.wstring(),identity=DllIdentity131(DllPath131(folder,requested)),ext=DllIdentity131(path.extension().wstring());std::wstring resolved;std::string report;
   if(ext==L".dll"){
    if(!fast&&IsFastJassDll131(path)){++result.skippedFast;continue;}
    if(dll.loaded.count(identity)){++result.already;continue;}
    if(dll.Load(folder,requested,resolved,report))++result.loaded;else ++result.failed;log("WHT_Lib: "+report);
   }else{
    if(mpq.loaded.count(identity)){++result.already;continue;}
    if(mpq.Load(folder,requested,[&](const std::wstring& path){return mount(path,archivePriority++);},resolved,report))++result.mounted;else ++result.failed;log("WHT_Lib: "+report);
   }
  }
  return result;
 }
 template<class Log> void StartDlls(const std::wstring& folder,bool fast,DllLoader131& dll,MpqLoader131& mpq,Log log){if(dllStarted||scanning)return;dllStarted=true;Scan(folder,fast,dll,mpq,[](const std::wstring&,unsigned){return uintptr_t(0);},log,false,true);}
 template<class Mount,class Log> void Start(const std::wstring& folder,bool fast,DllLoader131& dll,MpqLoader131& mpq,Mount mount,Log log){if(started||scanning)return;started=true;Scan(folder,fast,dll,mpq,mount,log,true,false);}
 static unsigned FastLoaded(const DllLoader131& dll){unsigned count=0;for(const auto& item:dll.loaded)if(IsFastJassDll131(item.first))++count;return count;}
 template<class Mount,class Save,class Reload,class Log> bool Command(const std::wstring& folder,const std::wstring& args,Config& config,DllLoader131& dll,MpqLoader131& mpq,Mount mount,Save save,Reload reload,Log log,std::string& report){
  bool enabled=false;if(!ParseFastJass131(args,config.fastJass,enabled)){report="Usage: #fastjass / #fj [0|1|off|on|false|true]";return false;}if(scanning||dll.loading||mpq.loading){report="Fast Jass: library initialization already in progress";return false;}
  const bool saved=save(enabled?L"1":L"0");reload();if(!saved||config.fastJass!=enabled){report="Fast Jass: setting could not be saved; library scan not started";return false;}
  const auto result=Scan(folder,enabled,dll,mpq,mount,log);const auto fastCount=FastLoaded(dll);
  report=enabled?"|CFFFCD211Fast Jass|r: |cff44ff3dOn|r":"|CFFFCD211Fast Jass|r: |cffff5c5cOff|r";
  report+="; WHT_Lib loaded "+std::to_string(result.loaded)+" DLL, "+std::to_string(result.mounted)+" archive, "+std::to_string(result.already)+" already loaded";
  if(enabled)report+=fastCount?"; FastJass DLLs loaded: "+std::to_string(fastCount):"; no FastJass DLL loaded";
  else if(fastCount)report+="; loaded FastJass DLLs remain until game restart";
  if(!result.Ok())report+="; some libraries could not be loaded (see WHT.log)";return result.Ok();
 }
};
}
