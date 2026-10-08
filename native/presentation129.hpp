#pragma once
#include <cstdint>
namespace wht {
// D3DPRESENT_PARAMETERS has 14 DWORDs; PresentationInterval is the last.
inline bool UnlockPresentation129(uint32_t* parameters,bool enabled){
 if(!parameters||!enabled)return false;
 parameters[13]=0x80000000u;return true; // D3DPRESENT_INTERVAL_IMMEDIATE
}
}
