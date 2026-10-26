#include <windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>
void check(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);}
int wmain(int argc,wchar_t** argv){try{check(argc==4,"arguments");
 if(std::filesystem::path(argv[1]).extension()==L".mix"){auto image=LoadLibraryExW(argv[1],nullptr,DONT_RESOLVE_DLL_REFERENCES);check(image!=nullptr,"Windows maps WHT.mix as an x86 DLL");check(GetProcAddress(image,"WHT_RebuildVersion")!=nullptr,"runtime export retained");FreeLibrary(image);}
 auto storm=LoadLibraryW(argv[2]);check(storm!=nullptr,"Storm loads");
 auto open=reinterpret_cast<BOOL(WINAPI*)(const char*,DWORD,DWORD,HANDLE*)>(GetProcAddress(storm,MAKEINTRESOURCEA(266)));
 auto close=reinterpret_cast<BOOL(WINAPI*)(HANDLE)>(GetProcAddress(storm,MAKEINTRESOURCEA(252)));
 auto fileOpen=reinterpret_cast<BOOL(WINAPI*)(HANDLE,const char*,DWORD,HANDLE*)>(GetProcAddress(storm,MAKEINTRESOURCEA(268)));
 auto fileClose=reinterpret_cast<BOOL(WINAPI*)(HANDLE)>(GetProcAddress(storm,MAKEINTRESOURCEA(253)));
 auto read=reinterpret_cast<BOOL(WINAPI*)(HANDLE,void*,DWORD,DWORD*,OVERLAPPED*)>(GetProcAddress(storm,MAKEINTRESOURCEA(269)));
 // A standalone Storm host needs the same standard MPQ crypt-table initialization
 // used by its archive opener in a game process. Resolve the verified call pattern.
 auto code=reinterpret_cast<const unsigned char*>(open);void* initTable=nullptr;
 const unsigned char seed[]={0xb8,1,0,0x10,0,0xe8};
 for(size_t n=0;n<160;++n)if(!memcmp(code+n,seed,sizeof(seed))){int relative=0;memcpy(&relative,code+n+6,4);initTable=const_cast<unsigned char*>(code+n+10+relative);break;}
 check(initTable!=nullptr,"Storm standard MPQ initializer identified");
 __asm { mov eax, 100001h }
 __asm { call initTable }
 check(open&&close&&fileOpen&&fileClose&&read,"Storm API present");HANDLE archive=nullptr;auto path=std::filesystem::path(argv[1]).string();check(open(path.c_str(),30,0,&archive)&&archive,"Storm mounts appended archive");
  for(auto name:{"BTNAllSkills.blp","BTNAllSkillsDow.blp","BTNAutoPick.blp","BTNAutoPickDow.blp","BTNDropItem.blp","BTNDropItemDow.blp","BTNSellItem.blp","BTNSellItemDow.blp","BTNWHT_Menu.blp","BTNWHT_Rec.blp","CommandCardHuman.tga","fonts_VN.ttf","FRIZQT__.ttf","InventoryHuman.tga","Transparent.tga","UI\\Widgets\\EscMenu\\Human\\human-options-button-border-up.blp","UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp","WHTUI_GreyBackground.tga"}){HANDLE file=nullptr;check(fileOpen(archive,name,0,&file)&&file,"bundled resource found");if(std::string(name)=="BTNWHT_Rec.blp"){std::ifstream in(std::filesystem::path(argv[3]),std::ios::binary);std::vector<char> expected((std::istreambuf_iterator<char>(in)),{}),actual(expected.size());DWORD got=0;check(read(file,actual.data(),DWORD(actual.size()),&got,nullptr)&&got==actual.size()&&actual==expected,"bundled icon byte-for-byte matches source");}fileClose(file);}
 for(auto name:{"AARDV.ttf","WHTUI_FRIZQT__.ttf","WHTMacroRecord.tga","WHT_IJL15_Original.dll"}){HANDLE file=nullptr;const bool found=fileOpen(archive,name,0,&file)!=FALSE;if(file)fileClose(file);check(!found,"unused asset absent from bundled archive");}
 {HANDLE file=nullptr;check(fileOpen(archive,"FRIZQT__.ttf",0,&file)&&file,"canonical FRIZQT font retained");fileClose(file);}
 close(archive);FreeLibrary(storm);std::cout<<"PASS Windows PE mapping and actual Storm bundled resources\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
