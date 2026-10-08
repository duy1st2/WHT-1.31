#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
#include <cstdio>
#include "overlay_frames.hpp"

namespace wht {

inline std::string FormatMatchTimer(uint32_t seconds) {
    uint32_t hrs = seconds / 3600;
    uint32_t mins = (seconds / 60) % 60;
    uint32_t secs = seconds % 60;
    char buf[128] = {0};
    std::snprintf(buf, sizeof(buf), "|cfffcd211%02u|cffffffff:|cfffcd211%02u|cffffffff:|cfffcd211%02u|r", hrs, mins, secs);
    return std::string(buf);
}

inline std::string FormatMatchTimerShadow(uint32_t seconds) {
    uint32_t hrs = seconds / 3600;
    uint32_t mins = (seconds / 60) % 60;
    uint32_t secs = seconds % 60;
    char buf[64] = {0};
    std::snprintf(buf, sizeof(buf), "|cff000000%02u:%02u:%02u|r", hrs, mins, secs);
    return std::string(buf);
}

inline std::string FormatMatchTimerPlain(uint32_t seconds) {
    uint32_t hrs = seconds / 3600;
    uint32_t mins = (seconds / 60) % 60;
    uint32_t secs = seconds % 60;
    char buf[32] = {0};
    std::snprintf(buf, sizeof(buf), "%02u:%02u:%02u", hrs, mins, secs);
    return std::string(buf);
}

inline bool TryReadGameClockSeconds(uintptr_t game,uint32_t& seconds) {
    seconds=0;
    if (!game) return false;
    __try {
        uintptr_t clockPtr = *reinterpret_cast<uintptr_t*>(game + ActiveGameOffsets().matchTimer);
        if (clockPtr >= 0x10000 && clockPtr < 0x7FFF0000) {
            uintptr_t scheduler=*reinterpret_cast<uintptr_t*>(clockPtr+0x40);
            if(scheduler<0x10000 || scheduler>=0x7FFF0000)return false;
            float currTime = *reinterpret_cast<float*>(scheduler + 0x40);
            if (currTime >= 0.0f && currTime < 864000.0f) {
                seconds=static_cast<uint32_t>(currTime);return true;
            }
        }
    } __except (1) {}
    return false;
}
inline uint32_t ReadGameClockSeconds(uintptr_t game){uint32_t seconds=0;TryReadGameClockSeconds(game,seconds);return seconds;}

class MatchTimerOverlay {
public:
    void* timerFrame = nullptr;
    void* timerShadow = nullptr;
    void* timerMain = nullptr;
    NativeFrameApi api{};
    uintptr_t currentRoot = 0;
    bool initialized = false;
    bool failed = false;
    ULONGLONG matchStartTime = 0;
    bool lastHideTimeOfDay = false;

    void Reset() {
        // Engine parent owns these objects. Reset only forgets handles on teardown.
        timerFrame = nullptr;
        timerShadow = nullptr;
        timerMain = nullptr;
        currentRoot = 0;
        initialized = false;
        failed = false;
        matchStartTime = 0;
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
            timerFrame = api.frameCtor(fMem, reinterpret_cast<void*>(owner));
            if (!timerFrame) return false;
            NativeFrameHide(timerFrame);

            if (api.setWidth) api.setWidth(timerFrame, 0.12f);
            if (api.setHeight) api.setHeight(timerFrame, 0.025f);

            float posX = lastHideTimeOfDay ? 0.40f : 0.34f;
            float posY = 0.568f;
            if (api.setAbsPoint) {
                api.setAbsPoint(timerFrame, 4, posX, posY, 1);
            }

            timerShadow = NativeCreateFontString(api, timerFrame, 0.016f, "");
            if (timerShadow) {
                if (api.setRelPoint) {
                    api.setRelPoint(timerShadow, 4, timerFrame, 4, 0.001f, -0.001f, 1);
                }
                if (api.setTextAlign) {
                    api.setTextAlign(timerShadow, 1); // center
                }
            }

            timerMain = NativeCreateFontString(api, timerFrame, 0.016f, "");
            if (timerMain) {
                if (api.setRelPoint) {
                    api.setRelPoint(timerMain, 4, timerFrame, 4, 0.0f, 0.0f, 1);
                }
                if (api.setTextAlign) {
                    api.setTextAlign(timerMain, 1); // center
                }
            }

            if (!timerShadow || !timerMain) return false;
            initialized = true;
            return true;
        } __except (1) {
            return false;
        }
    }

    void Apply(
        uintptr_t game,
        uintptr_t root,
        bool showTimer,
        bool hideTimeOfDay)
    {
        if (!game || !root) {
            if (initialized) Reset();
            return;
        }

        if (root != currentRoot) Reset();
        if (failed || (!initialized && !showTimer)) return;
        if (root != currentRoot || !initialized) {
            if (!Init(game, root)) { currentRoot=root;failed=true;NativeFrameHide(timerFrame);return; }
        }

        if (matchStartTime == 0) {
            matchStartTime = GetTickCount64();
        }

        if (lastHideTimeOfDay != hideTimeOfDay) {
            lastHideTimeOfDay = hideTimeOfDay;
            float posX = hideTimeOfDay ? 0.40f : 0.34f;
            float posY = 0.568f;
            if (timerFrame && api.setAbsPoint) {
                api.setAbsPoint(timerFrame, 4, posX, posY, 1);
            }
        }

        if (!showTimer) {
            if (timerFrame) NativeFrameHide(timerFrame);
            return;
        }

        if (timerFrame) {
            NativeFrameShow(timerFrame);
        }

        uint32_t elapsed = ReadGameClockSeconds(game);
        if (elapsed == 0 && matchStartTime != 0) {
            elapsed = static_cast<uint32_t>((GetTickCount64() - matchStartTime) / 1000);
        }

        std::string shadowText = FormatMatchTimerShadow(elapsed);
        std::string mainText = FormatMatchTimer(elapsed);

        if (timerShadow && api.setText) api.setText(timerShadow, shadowText.c_str());
        if (timerMain && api.setText) api.setText(timerMain, mainText.c_str());
    }
};

} // namespace wht
