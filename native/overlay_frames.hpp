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
};
inline NativeFrameLayout nativeFrameLayout{};
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

inline constexpr uint32_t kToolFrameLevel = 8;
inline constexpr int kNativeOverlayLayer = 4;

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
        auto& overlayEnabled = *reinterpret_cast<uint32_t*>(at + nativeFrameLayout.layers + kNativeOverlayLayer * 4);
        if (level >= kToolFrameLevel && overlayEnabled) return true;
        const auto hide = *reinterpret_cast<void(__thiscall**)(void*)>(vtable + nativeFrameLayout.hide);
        const auto show = *reinterpret_cast<void(__thiscall**)(void*)>(vtable + nativeFrameLayout.show);
        if (!hide || !show) return false;
        auto& visible = *reinterpret_cast<uint32_t*>(at + nativeFrameLayout.visible);
        const auto wasVisible = visible;
        visible = 0;
        hide(frame);
        if (level < kToolFrameLevel) level = kToolFrameLevel;
        // Enable the same per-layer gate used by native EnableDrawLayer (6093D0).
        // Show re-registers/invalidates all five render buckets after this write.
        overlayEnabled = 1;
        visible = wasVisible;
        if (wasVisible) show(frame); // Engine still checks the parent's visibility.
        return true;
    } __except (1) { return false; }
}

inline void NativeFrameShow(void* frame) {
    if (!frame) return;
    __try {
        uintptr_t vtable = *reinterpret_cast<uintptr_t*>(frame);
        if (vtable) {
            *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(frame) + nativeFrameLayout.visible) = 1;
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
            *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(frame) + nativeFrameLayout.visible) = 0;
            auto hideFn = *reinterpret_cast<void(__thiscall**)(void*)>(vtable + nativeFrameLayout.hide);
            if (hideFn) hideFn(frame);
        }
    } __except (1) {}
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
        void* backdrop = api.alloc(0x50, "CSimpleBackdrop", 1, 0);
        if (!backdrop) return;
        backdrop = api.backdropCtor(backdrop);
        if (!backdrop) return;
        // CBackdropFrame owns and releases its style.
        *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(frame) + nativeFrameLayout.backdrop) = reinterpret_cast<uintptr_t>(backdrop);

        *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(backdrop) + 0x2c) = (borderFlags != 0 ? borderFlags : 0xff);
        *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(backdrop) + 0x30) = 1;

        if (bgPath && *bgPath) {
            api.stringAssign(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(backdrop) + 0x14), bgPath);
        }
        if (borderPath && *borderPath) {
            api.stringAssign(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(backdrop) + 0x20), borderPath);
        }

        *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(backdrop) + 0x38) = borderSize;
        *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(backdrop) + 0x40) = inset;
        *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(backdrop) + 0x44) = inset;
        *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(backdrop) + 0x48) = inset;
        *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(backdrop) + 0x4c) = inset;

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
                "WHTUI_FRIZQT__.ttf",
                "FRIZQT__.ttf",
                "fonts_VN.ttf",
                "AARDV.ttf",
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
