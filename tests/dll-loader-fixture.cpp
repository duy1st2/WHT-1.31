#include <windows.h>
extern "C" __declspec(dllexport) int WHT131PluginFixture(){return 131;}
BOOL WINAPI DllMain(HINSTANCE,DWORD,void*){return TRUE;}
