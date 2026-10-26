#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
// Shared native primitives extracted from the incoming resource_panel.hpp.
// Show/Hide apply only to CBackdropFrame, never CSimpleFontString (+0x90 is font size).
namespace wht {
// Each runtime owns its layout. 1.29 adds a base-class field and a vtable slot.
struct NativeFrameLayout {
    size_t frameSize=0x124,fontSize=0xc4,textureSize=0xe8;
    unsigned level=0x84,visible=0x90,layers=0xb8,backdrop=0xcc,hide=0x64,show=0x68;
    uintptr_t simpleVtable=0;
    bool byteFlags=false;
};
inline NativeFrameLayout nativeFrameLayout{};
inline uint32_t NativeVisibleValue(void* frame){auto at=static_cast<unsigned char*>(frame)+nativeFrameLayout.visible;
 return nativeFrameLayout.byteFlags?*at:*reinterpret_cast<uint32_t*>(at);}
inline void NativeWriteVisible(void* frame,uint32_t value){auto at=static_cast<unsigned char*>(frame)+nativeFrameLayout.visible;
 if(nativeFrameLayout.byteFlags)*at=static_cast<unsigned char>(value);else *reinterpret_cast<uint32_t*>(at)=value;}
struct NativeFrameApi {
    void* (__stdcall *alloc)(size_t, const char*, int, int) = nullptr;
    void* (__thiscall *frameCtor)(void*, void*) = nullptr;
    void* (__thiscall *backdropCtor)(void*) = nullptr;
    void  (__thiscall *setBackdrop)(void*, void*) = nullptr;
    void  (__thiscall *stringAssign)(void*, const char*) = nullptr;
    void* (__thiscall *fontCtor)(void*, void*, int, int) = nullptr;
    uint32_t (__thiscall *setFont)(void*, const char*, float, int) = nullptr;
    void  (__thiscall *setTextAlign)(void*, int) = nullptr;
    void  (__thiscall *setText)(void*, const char*) = nullptr;
    void* (__thiscall *textureCtor)(void*, void*, int, int) = nullptr;
    void  (__thiscall *setTexture)(void*, const char*, int) = nullptr;
    void  (__thiscall *setWidth)(void*, float) = nullptr;
    void  (__thiscall *setHeight)(void*, float) = nullptr;
    void  (__thiscall *setAbsPoint)(void*, int, float, float, int) = nullptr;
    void  (__thiscall *setRelPoint)(void*, int, void*, int, float, float, int) = nullptr;
};
struct ExtendedFrameApi : public NativeFrameApi {void (__thiscall *setVisible)(void*,int)=nullptr;};

inline constexpr uint32_t kToolFrameLevel = 8;
// Reserve the upper three native buckets for the main settings menu.
inline constexpr uint32_t kToolFeatureFrameLevel = 6;
inline constexpr uint32_t kToolMenuPanelLevel = 7;
inline constexpr uint32_t kToolMenuControlLevel = 8;
inline constexpr uint32_t kToolMenuPopupLevel = 9;
inline constexpr int kNativeOverlayLayer = 4;

inline bool NativeSetOwnedFrameLevel(void* frame,uintptr_t game,uintptr_t table,uint32_t target){
 if(!frame||!game||!table||target>9)return false;
 __try{auto at=reinterpret_cast<uintptr_t>(frame);auto vt=*reinterpret_cast<uintptr_t*>(at);
  if(vt!=game+table)return false;
  auto& level=*reinterpret_cast<uint32_t*>(at+nativeFrameLayout.level);if(level>9)return false;if(level==target)return true;
  auto hide=*reinterpret_cast<void(__thiscall**)(void*)>(vt+nativeFrameLayout.hide);
  auto show=*reinterpret_cast<void(__thiscall**)(void*)>(vt+nativeFrameLayout.show);if(!hide||!show)return false;
  auto visible=NativeVisibleValue(frame);NativeWriteVisible(frame,0);hide(frame);level=target;NativeWriteVisible(frame,visible);if(visible)show(frame);return true;
 }__except(1){return false;}
}

// Exact 1.24.4.6387 CSimpleFrame/CSimpleButton ABI, not a CLayoutFrame
// or CSimpleRegion API. +0x84 is the manager's frame bucket (0..9).
// Hide removes both render and event registrations; Show rebuilds them with
// the new level. Writing the field alone leaves the old draw/click lists stale.
// The ctor disables layer 4 at +0xc8; it must be enabled for overlay icons/fonts.
// Call only on our own hosts, before creating any child frames.
inline bool NativeBringToolFrameToFront(void* frame, uintptr_t game) {
    if (!frame || !game) return false;
    __try {
        const auto at = reinterpret_cast<uintptr_t>(frame);
        const auto vtable = *reinterpret_cast<uintptr_t*>(at);
        if ((!nativeFrameLayout.simpleVtable || vtable != game + nativeFrameLayout.simpleVtable) &&
            vtable != game + 0x97fc5c && vtable != game + 0x97f934 &&
            vtable != game + 0x96dfb4 && vtable != game + 0x96dc8c) return false;
        auto& level = *reinterpret_cast<uint32_t*>(at + nativeFrameLayout.level);
        if (level > 9) return false;
        auto layerAt=at+nativeFrameLayout.layers+kNativeOverlayLayer*(nativeFrameLayout.byteFlags?1:4);
        const auto overlayEnabled=nativeFrameLayout.byteFlags?*reinterpret_cast<uint8_t*>(layerAt):*reinterpret_cast<uint32_t*>(layerAt);
        if (level >= kToolFrameLevel && overlayEnabled) return true;
        const auto hide = *reinterpret_cast<void(__thiscall**)(void*)>(vtable + nativeFrameLayout.hide);
        const auto show = *reinterpret_cast<void(__thiscall**)(void*)>(vtable + nativeFrameLayout.show);
        if (!hide || !show) return false;
        const auto wasVisible = NativeVisibleValue(frame);
        NativeWriteVisible(frame,0);
        hide(frame);
        if (level < kToolFrameLevel) level = kToolFrameLevel;
        // Enable the same per-layer gate used by native EnableDrawLayer (6093D0).
        // Show re-registers/invalidates all five render buckets after this write.
        if(nativeFrameLayout.byteFlags)*reinterpret_cast<uint8_t*>(layerAt)=1;else *reinterpret_cast<uint32_t*>(layerAt)=1;
        NativeWriteVisible(frame,wasVisible);
        if (wasVisible) show(frame); // Engine still checks the parent's visibility.
        return true;
    } __except (1) { return false; }
}

inline void NativeFrameShow(void* frame) {
    if (!frame) return;
    __try {
        uintptr_t vtable = *reinterpret_cast<uintptr_t*>(frame);
        if (vtable) {
            NativeWriteVisible(frame,1);
            auto showFn = *reinterpret_cast<void(__thiscall**)(void*)>(vtable + nativeFrameLayout.show);
            if (showFn) showFn(frame);
        }
    } __except (1) {}
}

inline void NativeFrameHide(void* frame) {
    if (!frame) return;
    __try {
        uintptr_t vtable = *reinterpret_cast<uintptr_t*>(frame);
        if (vtable) {
            NativeWriteVisible(frame,0);
            auto hideFn = *reinterpret_cast<void(__thiscall**)(void*)>(vtable + nativeFrameLayout.hide);
            if (hideFn) hideFn(frame);
        }
    } __except (1) {}
}
inline void NativeBackdropGeometry(void* backdrop,float borderSize,float inset){
 auto at=static_cast<unsigned char*>(backdrop);
 if(sizeof(void*)==8){*reinterpret_cast<float*>(at+0x60)=borderSize;for(unsigned n=0;n<4;++n)*reinterpret_cast<float*>(at+0x68+n*4)=inset;return;}
 // 1.31 moves EdgeSize and the four insets four bytes earlier. +38 is
 // BgTileSize there; overwriting it produces tiled/bleeding backgrounds.
 *reinterpret_cast<float*>(at+(nativeFrameLayout.byteFlags?0x34:0x38))=borderSize;
 const auto start=nativeFrameLayout.byteFlags?0x3c:0x40;
 for(unsigned n=0;n<4;++n)*reinterpret_cast<float*>(at+start+n*4)=inset;
}

inline void NativeSetFrameBackdropEx(
    const NativeFrameApi& api,
    void* frame,
    const char* bgPath,
    const char* borderPath,
    float borderSize,
    float inset,
    uint32_t borderFlags = 0xff)
{
    if (!api.alloc || !api.backdropCtor || !api.setBackdrop || !api.stringAssign || !frame) return;
    __try {
        void* backdrop = api.alloc(sizeof(void*)==8?0x78:0x50, "CSimpleBackdrop", 1, 0);
        if (!backdrop) return;
        backdrop = api.backdropCtor(backdrop);
        if (!backdrop) return;
        // CBackdropFrame owns and releases its style.
        *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(frame) + nativeFrameLayout.backdrop) = reinterpret_cast<uintptr_t>(backdrop);

        *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(backdrop) + (sizeof(void*)==8?0x58:0x2c)) = (borderFlags != 0 ? borderFlags : 0xff);
        *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(backdrop) + (sizeof(void*)==8?0x5c:0x30)) = 1;

        if (bgPath && *bgPath) {
            api.stringAssign(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(backdrop) + (sizeof(void*)==8?0x28:0x14)), bgPath);
        }
        if (borderPath && *borderPath) {
            api.stringAssign(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(backdrop) + (sizeof(void*)==8?0x40:0x20)), borderPath);
        }

        NativeBackdropGeometry(backdrop,borderSize,inset);

        api.setBackdrop(frame, backdrop);
    } __except (1) {}
}

inline void NativeSetFrameBackdrop(
    const NativeFrameApi& api,
    void* frame,
    const char* bgPath,
    const char* borderPath,
    float borderSize,
    float inset)
{
    NativeSetFrameBackdropEx(api, frame, bgPath, borderPath, borderSize, inset, 0xff);
}

inline void* NativeCreateFontString(
    const NativeFrameApi& api,
    void* parent,
    float fontHeight,
    const char* text = "",
    int layer = 2)
{
    if (!api.alloc || !api.fontCtor || !parent || layer < 0 || layer > 4) return nullptr;
    __try {
        void* fontMem = api.alloc(nativeFrameLayout.fontSize, "CSimpleFontString", 1, 0);
        if (!fontMem) return nullptr;
        void* font = api.fontCtor(fontMem, parent, layer, 1);
        if (!font) return nullptr;

        if (api.setFont) {
            const char* fontCandidates[] = {
                "FRIZQT__.ttf",
                "fonts_VN.ttf",
                "Fonts\\FRIZQT__.TTF"
            };
            for (const char* candidate : fontCandidates) {
                __try {
                    if (api.setFont(font, candidate, fontHeight, 1)) {
                        break;
                    }
                } __except (1) {}
            }
        }
        if (api.setTextAlign) {
            api.setTextAlign(font, 1);
        }
        if (text && *text && api.setText) {
            api.setText(font, text);
        }
        return font;
    } __except (1) {}
    return nullptr;
}

inline void SafeSetText(const NativeFrameApi& api, void* font, const char* text) {
    if (!font || !api.setText) return;
    __try {
        api.setText(font, text ? text : "");
    } __except (1) {}
}


} // namespace wht
