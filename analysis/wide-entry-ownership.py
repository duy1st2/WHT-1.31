from pathlib import Path
p=Path('WHT-1.31/native/wide_screen131.hpp');s=p.read_text();marker='inline bool InstallWideScreen131('
helper='''inline bool WideScreenOwnedEntry131(uintptr_t target){if(!target)return false;__try{const auto* code=reinterpret_cast<const unsigned char*>(target);if(code[0]!=0xe9)return false;const auto destination=target+5+*reinterpret_cast<const int32_t*>(code+1);
#ifdef _WIN64
 const unsigned char relay[]={0xff,0x25,0,0,0,0};return !memcmp(reinterpret_cast<const void*>(destination),relay,sizeof(relay))&&*reinterpret_cast<const uintptr_t*>(destination+6)==reinterpret_cast<uintptr_t>(WideScreenHook131);
#else
 return destination==reinterpret_cast<uintptr_t>(WideScreenHook131);
#endif
 }__except(1){return false;}}
'''
s=s.replace(marker,helper+marker,1);s=s.replace('build==wideScreenBuild131&&WideScreenCode131','build==wideScreenBuild131&&WideScreenOwnedEntry131(base+p->projection)&&WideScreenCode131');s=s.replace('wideScreenReady131=true;return WideScreenCode131','wideScreenReady131=true;return WideScreenOwnedEntry131(base+p->projection)&&WideScreenCode131');p.write_text(s)
p=Path('WHT-1.31/tests/wide-screen-tests.cpp');s=p.read_text(encoding='utf-8-sig');s=s.replace('wht::wideScreenDivisorValue131.store(4.f/3.f);','Check(wht::WideScreenOwnedEntry131(reinterpret_cast<uintptr_t>(target)),"owned detour destination");wht::wideScreenDivisorValue131.store(4.f/3.f);');p.write_text(s,encoding='utf-8-sig')
