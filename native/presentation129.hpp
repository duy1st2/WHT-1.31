#pragma once
#include <cstdint>
namespace wht {
inline bool EffectiveUnlock129(bool local,bool unlock,bool vsync){return local&&unlock&&!vsync;}
inline bool ConfigureVSync129(uint32_t* parameters,bool allowed,bool vsync){
 if(!parameters||!allowed)return false;
 parameters[13]=vsync?1u:0x80000000u;return true;
}
// D3DPRESENT_PARAMETERS has 14 DWORDs; PresentationInterval is the last.
inline bool UnlockPresentation129(uint32_t* parameters,bool enabled){
 if(!parameters||!enabled)return false;
 parameters[13]=0x80000000u;return true; // D3DPRESENT_INTERVAL_IMMEDIATE
}
}
