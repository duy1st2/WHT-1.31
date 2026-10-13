#pragma once
#include "overlay_frames.hpp"
#include "common.hpp"
#include "courier_mth_manager.hpp"
#include <windows.h>
#include <mmsystem.h>
#include <cstdint>
#include <vector>
#include <cstdio>

namespace wht {

struct MthButtonUI {
    void* frame = nullptr;
    void* text = nullptr;
    float x = 0.0f;       // Bottom-right X coordinate
    float y = 0.142f;     // Bottom-right Y coordinate
    float width = 0.038f;
    float height = 0.024f;
    float borderSize = 0.0125f;
    float inset = 0.0045f;
    int comboType = -1;   // 0=Shield, 1=Ice, 2=Wild, 3=Frozen, 4=Phoenix, 5=Goku
    const char* label = "";
    bool enabled = true;
    bool isPressed = false;
    ULONGLONG pressedUntil = 0;
    bool shown=false;float appliedX=-1,appliedY=-1;
    uint32_t labelColor = 0; // Last applied color; avoid native text writes every tick.
    bool labelEnglish = false;
};

inline void* SafeAllocAndCtorFrame(const ExtendedFrameApi& api, void* parent, float width, float height, float x, float y, float borderSize = 0.0125f, float inset = 0.0045f) {
    __try {
        if (!api.alloc || !api.frameCtor || !parent) return nullptr;
        void* fMem = api.alloc(0x124, "CSimpleFrame", 1, 0);
        if (!fMem) return nullptr;
        void* frame = api.frameCtor(fMem, parent);
        if (!frame) return nullptr;

        // Set dimensions and anchor points:
        // Point 0 (TOPLEFT): (x - width, y + height)
        // Point 8 (BOTTOMRIGHT): (x, y)
        if (api.setWidth) api.setWidth(frame, width);
        if (api.setHeight) api.setHeight(frame, height);
        if (api.setAbsPoint) {
            api.setAbsPoint(frame, 0, x - width, y + height, 1);
            api.setAbsPoint(frame, 8, x, y, 1);
        }

        // Exact ETS 45-degree chamfered gold beveled corners & slate backdrop
        NativeSetFrameBackdropEx(api, frame,
            "UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp",
            "UI\\Widgets\\EscMenu\\Human\\human-options-button-border-up.blp",
            borderSize, inset, 0xFF);

        NativeFrameShow(frame);
        return frame;
    } __except (1) {
        return nullptr;
    }
}

inline void* SafeCreateButtonLabel(const ExtendedFrameApi& api, void* frame, const char* label) {
    __try {
        if (!frame) return nullptr;
        char buf[64];
        sprintf_s(buf, sizeof(buf), "|cffffcc00%s|r", label ? label : "");
        void* text = NativeCreateFontString(api, frame, 0.0100f, buf, kNativeOverlayLayer);
        if (text) {
            if (api.setTextAlign) api.setTextAlign(text, 1); // CENTER align
            if (api.setRelPoint) api.setRelPoint(text, 4, frame, 4, 0.0f, 0.0005f, 1);
            // Font visibility is inherited from its host; it is not a frame.
        }
        return text;
    } __except (1) {
        return nullptr;
    }
}

class MthButtonOverlay {
public:
    uintptr_t currentConsoleUI = 0;
    bool initialized = false;
    bool gokuActive = false;
    ExtendedFrameApi api{};
    std::vector<MthButtonUI> buttons;

    MthButtonOverlay() {
        // 6 MTH combo buttons (shifted left, width: 0.038f, height: 0.024f, Y: 0.142f, step: 0.041f)
        // Anchor: 8 (BOTTOMRIGHT)
        buttons.push_back({nullptr, nullptr, 0.250f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 0, "Shield", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.291f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 1, "Ice", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.332f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 2, "Wild", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.373f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 3, "Frozen", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.414f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 4, "Phoenix", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.455f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 5, "Goku", false, false, 0});

        // 6 MTH route buttons (Ga1..3, Soi1..3) matching authentic ETS.mix route triggers:
        buttons.push_back({nullptr, nullptr, 0.496f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 10, "Ga1", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.537f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 11, "Ga2", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.578f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 12, "Ga3", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.619f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 20, "Soi1", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.660f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 21, "Soi2", true, false, 0});
        buttons.push_back({nullptr, nullptr, 0.701f, 0.142f, 0.038f, 0.024f, 0.0125f, 0.0045f, 22, "Soi3", true, false, 0});
    }

    void Reset() {
        currentConsoleUI = 0;
        initialized = false;
        gokuActive = false;
        for (auto& b : buttons) {
            b.frame = nullptr;
            b.text = nullptr;
            b.isPressed = false;
            b.pressedUntil = 0;
            b.labelColor = 0;b.shown=false;b.appliedX=b.appliedY=-1;
        }
    }

    bool Init(uintptr_t game, uintptr_t parentHandle) {
        if (!game || !parentHandle) return false;
        HMODULE storm = GetModuleHandleW(L"Storm.dll");
        if (!storm) return false;
        api.alloc = reinterpret_cast<decltype(api.alloc)>(GetProcAddress(storm, reinterpret_cast<LPCSTR>(401)));
        if (!api.alloc) return false;

        const auto& o = ActiveGameOffsets();
        api.frameCtor = reinterpret_cast<decltype(api.frameCtor)>(game + o.frameCtor);
        api.backdropCtor = reinterpret_cast<decltype(api.backdropCtor)>(game + o.backdropCtor);
        api.setBackdrop = reinterpret_cast<decltype(api.setBackdrop)>(game + o.setBackdrop);
        api.stringAssign = reinterpret_cast<decltype(api.stringAssign)>(game + o.stringAssign);
        api.fontCtor = reinterpret_cast<decltype(api.fontCtor)>(game + o.fontCtor);
        api.setFont = reinterpret_cast<decltype(api.setFont)>(game + o.setFont);
        api.setTextAlign = reinterpret_cast<decltype(api.setTextAlign)>(game + o.setTextAlign);
        api.setText = reinterpret_cast<decltype(api.setText)>(game + o.setText);
        api.textureCtor = reinterpret_cast<decltype(api.textureCtor)>(game + o.textureCtor);
        api.setTexture = reinterpret_cast<decltype(api.setTexture)>(game + o.setTexture);
        api.setWidth = reinterpret_cast<decltype(api.setWidth)>(game + o.setWidth);
        api.setHeight = reinterpret_cast<decltype(api.setHeight)>(game + o.setHeight);
        api.setAbsPoint = reinterpret_cast<decltype(api.setAbsPoint)>(game + o.setAbsPoint);
        api.setRelPoint = reinterpret_cast<decltype(api.setRelPoint)>(game + o.setRelPoint);
        api.setVisible = reinterpret_cast<decltype(api.setVisible)>(game + o.setVisible);

        Reset();
        currentConsoleUI = parentHandle;

        void* parent = reinterpret_cast<void*>(parentHandle);
        for (auto& b : buttons) {
            void* frame = SafeAllocAndCtorFrame(api, parent, b.width, b.height, b.x, b.y, b.borderSize, b.inset);
            if (!frame) continue;
            NativeBringToolFrameToFront(frame, game);
            b.frame = frame;
            b.text = SafeCreateButtonLabel(api, frame, b.label);
            if (b.text) b.labelColor = 0xffcc00;
            if (b.enabled) NativeFrameShow(b.frame); else NativeFrameHide(b.frame);
            b.shown=b.enabled;
        }

        initialized = true;
        return true;
    }

    void RefreshLabel(MthButtonUI& b) {
        // Goku is a toggle: its ON/OFF state wins over the transient press color.
        const uint32_t color = b.comboType == 5 ? (gokuActive ? 0x44ff3d : 0xffcc00)
                                               : (b.isPressed ? 0xff9900 : 0xffcc00);
        const bool english=toolEnglish.load();
        if (!b.text || !api.setText || (b.labelColor == color&&b.labelEnglish==english)) return;
        char buf[64];
        std::string label=b.label?b.label:"";
        if(english&&b.comboType>=10&&b.comboType<=12)label="Ch"+std::to_string(b.comboType-9);
        if(english&&b.comboType>=20&&b.comboType<=22)label="Wolf"+std::to_string(b.comboType-19);
        sprintf_s(buf, sizeof(buf), "|cff%06x%s|r", color, label.c_str());
        SafeSetText(api, b.text, buf);
        b.labelColor = color;
        b.labelEnglish = english;
    }

    void SyncAutoPickGoku(bool active) {
        gokuActive = active;
        for (auto& b : buttons) if (b.comboType == 5) RefreshLabel(b);
    }

    void Update(uintptr_t game, uintptr_t parentHandle, const Config& config, bool autoPickGokuOn) {
        if (!game || !parentHandle) {
            if (initialized) Reset();
            return;
        }

        const bool groupEnabled=config.mth49Enable || config.g17Enable;
        buttons[0].enabled = groupEnabled && config.shieldDraw;
        buttons[1].enabled = groupEnabled && config.iceDraw;
        buttons[2].enabled = groupEnabled && config.wildDraw;
        buttons[3].enabled = groupEnabled && config.frozenDraw;
        buttons[4].enabled = groupEnabled && config.phoenixDraw;
        buttons[5].enabled = groupEnabled && config.pickGokuDraw;
        if (buttons.size() >= 12) {
            buttons[6].enabled = groupEnabled && config.autoGaDraw1;
            buttons[7].enabled = groupEnabled && config.autoGaDraw2;
            buttons[8].enabled = groupEnabled && config.autoGaDraw3;
            buttons[9].enabled = groupEnabled && config.autoSoiDraw1;
            buttons[10].enabled = groupEnabled && config.autoSoiDraw2;
            buttons[11].enabled = groupEnabled && config.autoSoiDraw3;
        }

        if (!initialized || currentConsoleUI != parentHandle) {
            if (!Init(game, parentHandle)) return;
        }
        gokuActive = autoPickGokuOn;

        ULONGLONG now = GetTickCount64();
        for (auto& b : buttons) {
            if (b.isPressed && now >= b.pressedUntil) {
                b.isPressed = false;
            }
        }

        // Dynamically pack enabled buttons horizontally starting from 0.250f with step 0.041f
        float currentX = 0.250f;
        const float step = 0.041f;
        for (auto& b : buttons) {
            if (b.enabled) {
                b.x = currentX;
                currentX += step;
            }
        }

        for (auto& b : buttons) {
            RefreshLabel(b);
            if (!b.frame) continue;
            __try {
                if (b.enabled) {
                    float x=b.x-b.width+(b.isPressed?0.0012f:0.0f);
                    float y=b.y+b.height+(b.isPressed?-0.0012f:0.0f);
                    if(api.setAbsPoint && (x!=b.appliedX || y!=b.appliedY)){
                        api.setAbsPoint(b.frame,0,x,y,1);
                        api.setAbsPoint(b.frame,8,x+b.width,y-b.height,1);
                        b.appliedX=x;b.appliedY=y;
                    }
                    if(!b.shown){NativeFrameShow(b.frame);b.shown=true;}
                }else if(b.shown){NativeFrameHide(b.frame);b.shown=false;}
            } __except(1) {}
        }
    }

    bool HandleClick(POINT pt, int clientW, int clientH, uintptr_t game, uintptr_t root,
                     void* (*jassLookup)(const char*),
                     bool (*clickDirectFn)(int),
                     bool (*clickBtnFn)(bool, int, bool),
                     CourierMthManager& mthManager,
                     const Config& config) {
        if(!config.mth49Enable && !config.g17Enable)return false;
        if (clientW <= 0 || clientH <= 0 || !root || !game) return false;

        // Disjoint, accurate screen coordinates mapping without pillarbox false-positives
        float uiX = static_cast<float>(pt.x) / static_cast<float>(clientW) * 0.8f;
        float uiY = static_cast<float>(clientH - pt.y) / static_cast<float>(clientH) * 0.6f;

        for (auto& b : buttons) {
            if (!b.enabled) continue;
            // Point 0 (TOPLEFT): (b.x - b.width, b.y + b.height)
            // Point 8 (BOTTOMRIGHT): (b.x, b.y)
            float left = b.x - b.width;
            float right = b.x;
            float bottom = b.y;
            float top = b.y + b.height;

            if (uiX >= left && uiX <= right && uiY >= bottom && uiY <= top) {
                PlaySoundA("Sound\\Interface\\GlueScreenClick.wav", NULL, SND_ASYNC | SND_NODEFAULT);
                b.isPressed = true;
                b.pressedUntil = GetTickCount64() + 140;

                // Visual depression immediately
                if (api.setAbsPoint && b.frame) {
                    float dx = 0.0012f;
                    float dy = -0.0012f;
                    api.setAbsPoint(b.frame, 0, (b.x - b.width) + dx, (b.y + b.height) + dy, 1);
                    api.setAbsPoint(b.frame, 8, b.x + dx, b.y + dy, 1);
                }
                if (b.comboType != 5) RefreshLabel(b);

                if (b.comboType >= 0 && b.comboType <= 4) {
                    mthManager.ExecuteMthCombo(game, root, jassLookup, clickDirectFn, clickBtnFn, b.comboType);
                    return true;
                } else if (b.comboType == 5) {
                    mthManager.ToggleAutoPickGoku();
                    SyncAutoPickGoku(mthManager.autoPickGoku);
                    return true;
                } else if (b.comboType >= 10 && b.comboType <= 12) {
                    std::wstring outMsg;
                    mthManager.ExecuteMthRoute(0, b.comboType - 10, game, root, jassLookup, outMsg);
                    return true;
                } else if (b.comboType >= 20 && b.comboType <= 22) {
                    std::wstring outMsg;
                    mthManager.ExecuteMthRoute(1, b.comboType - 20, game, root, jassLookup, outMsg);
                    return true;
                }
            }
        }
        return false;
    }
};

} // namespace wht
