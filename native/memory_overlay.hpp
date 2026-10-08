#pragma once
#include <windows.h>
#include <psapi.h>
#include <cstdint>
#include <string>
#include <cstdio>
#include "overlay_frames.hpp"

namespace wht {

inline bool GetCurrentProcessMemoryUsage(float& outUsedMB, float& outPercent) {
    using GetProcessMemoryInfoFn = BOOL(WINAPI*)(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
    static GetProcessMemoryInfoFn fn = []() -> GetProcessMemoryInfoFn {
        HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
        if (k32) {
            auto p = reinterpret_cast<GetProcessMemoryInfoFn>(GetProcAddress(k32, "K32GetProcessMemoryInfo"));
            if (p) return p;
        }
        HMODULE psapi = LoadLibraryA("psapi.dll");
        if (psapi) {
            return reinterpret_cast<GetProcessMemoryInfoFn>(GetProcAddress(psapi, "GetProcessMemoryInfo"));
        }
        return nullptr;
    }();

    if (!fn) return false;

    PROCESS_MEMORY_COUNTERS pmc{};
    pmc.cb = sizeof(pmc);
    if (!fn(GetCurrentProcess(), &pmc, sizeof(pmc))) return false;

    outUsedMB = static_cast<float>(pmc.WorkingSetSize) / (1024.0f * 1024.0f);
    outPercent = (outUsedMB / 2048.0f) * 100.0f;
    return true;
}

// Exact format string from ETS.mix VA 10069ee0:
// "|cFFDCDA3CMemory(MB)|cffFFFFFF: |cFFDCDA3C%5.2f |cffFFFFFF/ |cFFDCDA3C2048 |cffFFFFFF(|cFFDCDA3C%.2f%%|cffFFFFFF)|r"
inline std::string FormatMemoryMain(float usedMB, float pct) {
    char buf[256] = {0};
    std::snprintf(buf, sizeof(buf),
        "|cFFDCDA3CMemory(MB)|cffFFFFFF: |cFFDCDA3C%5.2f |cffFFFFFF/ |cFFDCDA3C2048 |cffFFFFFF(|cFFDCDA3C%.2f%%|cffFFFFFF)|r",
        usedMB, pct);
    return std::string(buf);
}

inline std::string FormatMemoryShadow(float usedMB, float pct) {
    char buf[256] = {0};
    std::snprintf(buf, sizeof(buf),
        "|cff000000Memory(MB): %5.2f / 2048 (%.2f%%)|r",
        usedMB, pct);
    return std::string(buf);
}

class MemoryOverlay {
public:
    void* memFrame = nullptr;
    void* memShadow = nullptr;
    void* memMain = nullptr;
    NativeFrameApi api{};
    uintptr_t currentRoot = 0;
    bool initialized = false;
    bool failed = false;
    bool lastHideTimeOfDay = false;
    ULONGLONG lastUpdateTick = 0;
    std::string cachedMain;
    std::string cachedShadow;

    void Reset() {
        memFrame = nullptr;
        memShadow = nullptr;
        memMain = nullptr;
        currentRoot = 0;
        initialized = false;
        failed = false;
        lastUpdateTick = 0;
        cachedMain.clear();
        cachedShadow.clear();
    }

    bool Init(uintptr_t game, uintptr_t root) {
        if (!game || !root) return false;
        uintptr_t owner = 0;
        __try { owner = *reinterpret_cast<uintptr_t*>(root + 0x428); } __except (1) { return false; }
        if (!owner) return false;
        HMODULE storm = GetModuleHandleW(L"Storm.dll");
        if (!storm) return false;
        api.alloc = reinterpret_cast<decltype(api.alloc)>(GetProcAddress(storm, reinterpret_cast<LPCSTR>(401)));
        if (!api.alloc) return false;

        const auto& o = ActiveGameOffsets();
        api.frameCtor = reinterpret_cast<decltype(api.frameCtor)>(game + o.frameCtor);
        api.fontCtor = reinterpret_cast<decltype(api.fontCtor)>(game + o.fontCtor);
        api.setFont = reinterpret_cast<decltype(api.setFont)>(game + o.setFont);
        api.setTextAlign = reinterpret_cast<decltype(api.setTextAlign)>(game + o.setTextAlign);
        api.setText = reinterpret_cast<decltype(api.setText)>(game + o.setText);
        api.setWidth = reinterpret_cast<decltype(api.setWidth)>(game + o.setWidth);
        api.setHeight = reinterpret_cast<decltype(api.setHeight)>(game + o.setHeight);
        api.setAbsPoint = reinterpret_cast<decltype(api.setAbsPoint)>(game + o.setAbsPoint);
        api.setRelPoint = reinterpret_cast<decltype(api.setRelPoint)>(game + o.setRelPoint);

        Reset();
        currentRoot = root;

        __try {
            void* fMem = api.alloc(0x124, "CSimpleFrame", 1, 0);
            if (!fMem) return false;
            memFrame = api.frameCtor(fMem, reinterpret_cast<void*>(owner));
            if (!memFrame) return false;
            NativeFrameHide(memFrame);

            // In ETS.mix: Font size 0.016f (0x3c83126f)
            if (api.setWidth) api.setWidth(memFrame, 0.22f);
            if (api.setHeight) api.setHeight(memFrame, 0.025f);

            // Position from ETS.mix 10066e40.c line 96: Point 4, X = 0.34f (0.40f if timeOfDay hidden), Y = 0.550f
            float posX = lastHideTimeOfDay ? 0.40f : 0.34f;
            float posY = 0.550f;
            if (api.setAbsPoint) {
                api.setAbsPoint(memFrame, 4, posX, posY, 1);
            }

            memShadow = NativeCreateFontString(api, memFrame, 0.016f, "");
            if (memShadow) {
                if (api.setRelPoint) {
                    api.setRelPoint(memShadow, 4, memFrame, 4, 0.001f, -0.001f, 1);
                }
                if (api.setTextAlign) {
                    api.setTextAlign(memShadow, 1); // center
                }
            }

            memMain = NativeCreateFontString(api, memFrame, 0.016f, "");
            if (memMain) {
                if (api.setRelPoint) {
                    api.setRelPoint(memMain, 4, memFrame, 4, 0.0f, 0.0f, 1);
                }
                if (api.setTextAlign) {
                    api.setTextAlign(memMain, 1); // center
                }
            }

            if (!memShadow || !memMain) return false;
            initialized = true;
            return true;
        } __except (1) {
            return false;
        }
    }

    void Apply(uintptr_t game, uintptr_t root, bool showMem, bool hideTimeOfDay) {
        if (!game || !root) {
            if (initialized) Reset();
            return;
        }

        if (root != currentRoot) Reset();
        if (failed || (!initialized && !showMem)) return;
        if (root != currentRoot || !initialized) {
            if (!Init(game, root)) {
                currentRoot = root;
                failed = true;
                NativeFrameHide(memFrame);
                return;
            }
        }

        if (lastHideTimeOfDay != hideTimeOfDay) {
            lastHideTimeOfDay = hideTimeOfDay;
            float posX = hideTimeOfDay ? 0.40f : 0.34f;
            float posY = 0.550f;
            if (memFrame && api.setAbsPoint) {
                api.setAbsPoint(memFrame, 4, posX, posY, 1);
            }
        }

        if (!showMem) {
            if (memFrame) NativeFrameHide(memFrame);
            return;
        }

        if (memFrame) {
            NativeFrameShow(memFrame);
        }

        ULONGLONG now = GetTickCount64();
        if (now - lastUpdateTick >= 500 || cachedMain.empty()) {
            lastUpdateTick = now;
            float usedMB = 0.0f, pct = 0.0f;
            if (GetCurrentProcessMemoryUsage(usedMB, pct)) {
                cachedMain = FormatMemoryMain(usedMB, pct);
                cachedShadow = FormatMemoryShadow(usedMB, pct);
            }
        }

        if (memShadow && api.setText) api.setText(memShadow, cachedShadow.c_str());
        if (memMain && api.setText) api.setText(memMain, cachedMain.c_str());
    }
};

} // namespace wht
