#pragma once
#include "overlay_frames.hpp"
#include "common.hpp"
#include "item_manager.hpp"
#include "gameplay_commands.hpp"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <cstdint>
#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <algorithm>

namespace wht {

struct ExtendedToolbarApi : public NativeFrameApi {
    void (__thiscall *setVisible)(void*, int) = nullptr;
    void (__thiscall *setTextureColor)(void*, const uint32_t*) = nullptr;
    void* (__thiscall *buttonCtor)(void*, void*) = nullptr;
    void (__thiscall *setButtonTexture)(void*, int, const char*) = nullptr;
    void (__thiscall *setButtonState)(void*, int) = nullptr;
};

class HudActionToolbar;

struct ToolbarObserver {
    virtual void Unused0() {}
    virtual void Unused1() {}
    virtual void Unused2() {}
    virtual void __thiscall OnFrameEvent(void* eventObj);

    int actionId = -1;
    HudActionToolbar* toolbar = nullptr;
};

struct ToolbarButtonUI {
    void* button = nullptr;     // Native CSimpleButton
    void* frame = nullptr;      // CSimpleFrame fallback/host
    void* texture = nullptr;    // CSimpleTexture
    float x = 0.613f;           // Point 8 (BOTTOMRIGHT) X coordinate
    float y = 0.0f;             // Point 8 (BOTTOMRIGHT) Y coordinate
    float width = 0.016f;
    float height = 0.016f;
    const char* normalTex = "";
    const char* activeTex = "";
    const char* tooltip = "";
    bool enabled = true;
    bool active = false;
    bool pressed = false;
    int actionId = 0;           // 0: Auto Pick, 1: Drop All, 2: Sell All, 3: Increase All Skills
    ULONGLONG pressStarted = 0;
    ULONGLONG pressedUntil = 0;
    const char* appliedTexture = nullptr;
    uint32_t textureColor = 0;
};

class HudActionToolbar {
public:
    uintptr_t currentConsoleUI = 0;
    bool initialized = false;
    ExtendedToolbarApi api{};
    std::vector<ToolbarButtonUI> buttons;
    ToolbarObserver observers[4];
    void (*onActionCallback)(int actionId) = nullptr;
    DWORD lastClickTick = 0;
    int lastAction = -1;
    static constexpr ULONGLONG kPressAnimationMs = 180;

    static inline bool SafeIsExecutable(uintptr_t at) {
        MEMORY_BASIC_INFORMATION m{};
        return at && VirtualQuery(reinterpret_cast<void*>(at), &m, sizeof(m)) == sizeof(m) &&
            m.State == MEM_COMMIT && !(m.Protect & (PAGE_GUARD | PAGE_NOACCESS)) &&
            (m.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
    }

    static inline bool SafeIsReadable(const void* p, size_t n) {
        MEMORY_BASIC_INFORMATION m{};
        if (!p || !n || VirtualQuery(p, &m, sizeof(m)) != sizeof(m) || m.State != MEM_COMMIT ||
            (m.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
        uintptr_t start = reinterpret_cast<uintptr_t>(p);
        uintptr_t end = reinterpret_cast<uintptr_t>(m.BaseAddress) + m.RegionSize;
        return start <= end && n <= end - start;
    }

    HudActionToolbar() {
        for (int i = 0; i < 4; ++i) {
            observers[i].actionId = i;
            observers[i].toolbar = this;
        }
        // 4 HUD Action Toolbar buttons vertically stacked next to the inventory panel (X = 0.613f)
        // Scaled to width = 0.016f, height = 0.016f so they fit snugly without touching skills or stone pillar
        // Y coordinates: 0.111f, 0.083f, 0.055f, 0.027f
        buttons.push_back({nullptr, nullptr, nullptr, 0.613f, 0.111f, 0.016f, 0.016f,
                           "BTNAutoPick.blp", "BTNAutoPickDow.blp", "Auto Pick Item", true, false, false, 0});
        buttons.push_back({nullptr, nullptr, nullptr, 0.613f, 0.083f, 0.016f, 0.016f,
                           "BTNDropItem.blp", "BTNDropItemDow.blp", "Drop All Item", true, false, false, 1});
        buttons.push_back({nullptr, nullptr, nullptr, 0.613f, 0.055f, 0.016f, 0.016f,
                           "BTNSellItem.blp", "BTNSellItemDow.blp", "Sell All Item", true, false, false, 2});
        buttons.push_back({nullptr, nullptr, nullptr, 0.613f, 0.027f, 0.016f, 0.016f,
                           "BTNAllSkills.blp", "BTNAllSkillsDow.blp", "Increase All Skills", true, false, false, 3});
    }

    void Reset() {
        currentConsoleUI = 0;
        initialized = false;
        for (auto& b : buttons) {
            b.button = nullptr;
            b.frame = nullptr;
            b.texture = nullptr;
            b.pressed = false;
            b.pressStarted = b.pressedUntil = 0;
            b.appliedTexture = nullptr;
            b.textureColor = 0;
        }
        lastClickTick = 0;
        lastAction = -1;
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
        api.setTextureColor = reinterpret_cast<decltype(api.setTextureColor)>(game + o.setTextureColor);
        api.buttonCtor = reinterpret_cast<decltype(api.buttonCtor)>(game + o.buttonCtor);
        api.setButtonTexture = reinterpret_cast<decltype(api.setButtonTexture)>(game + o.setButtonTexture);
        api.setButtonState = reinterpret_cast<decltype(api.setButtonState)>(game + o.setButtonState);

        Reset();
        currentConsoleUI = parentHandle;

        void* parent = reinterpret_cast<void*>(parentHandle);
        for (size_t i = 0; i < buttons.size(); ++i) {
            auto& b = buttons[i];
            __try {
                if (!api.alloc) continue;

                // 1. Create native Warcraft III CSimpleButton
                if (api.buttonCtor) {
                    void* btnMem = api.alloc(0x168, "CSimpleButton", 1, 0);
                    if (btnMem) {
                        void* btn = api.buttonCtor(btnMem, parent);
                        if (btn) {
                            NativeBringToolFrameToFront(btn, game);
                            if (api.setWidth) api.setWidth(btn, b.width);
                            if (api.setHeight) api.setHeight(btn, b.height);
                            if (api.setAbsPoint) {
                                api.setAbsPoint(btn, 0, b.x - b.width, b.y + b.height, 1);
                                api.setAbsPoint(btn, 8, b.x, b.y, 1);
                            }
                            if (api.setButtonTexture) {
                                api.setButtonTexture(btn, 0, b.normalTex);
                                api.setButtonTexture(btn, 1, b.activeTex);
                                api.setButtonTexture(btn, 2, b.activeTex);
                            }
                            if (api.setButtonState) {
                                api.setButtonState(btn, 1);
                            }
                            if (api.setVisible) {
                                api.setVisible(btn, b.enabled ? 1 : 0);
                            }
                            if (i < 4) {
                                *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(btn) + 0x124) = &observers[i];
                                *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(btn) + 0x128) = 3000;
                            }
                            b.button = btn;
                            b.frame = btn;
                        }
                    }
                }

                // 2. Supplementary CSimpleFrame if button failed
                if (!b.frame && api.frameCtor) {
                    void* fMem = api.alloc(0x124, "CSimpleFrame", 1, 0);
                    if (fMem) {
                        void* frame = api.frameCtor(fMem, parent);
                        if (frame) {
                            NativeBringToolFrameToFront(frame, game);
                            if (api.setWidth) api.setWidth(frame, b.width);
                            if (api.setHeight) api.setHeight(frame, b.height);
                            if (api.setAbsPoint) {
                                api.setAbsPoint(frame, 0, b.x - b.width, b.y + b.height, 1);
                                api.setAbsPoint(frame, 8, b.x, b.y, 1);
                            }
                            if (api.setVisible) api.setVisible(frame, b.enabled ? 1 : 0);
                            b.frame = frame;
                        }
                    }
                }

                // 3. Clean CSimpleTexture icon for dynamic tint and custom visual depression
                void* texParent = b.frame ? b.frame : parent;
                if (api.textureCtor) {
                    void* texMem = api.alloc(0xe8, "CSimpleTexture", 1, 0);
                    if (texMem) {
                        void* tex = api.textureCtor(texMem, texParent, kNativeOverlayLayer, 1);
                        if (tex) {
                            *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(tex) + 0x58) = b.width;
                            *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(tex) + 0x5c) = b.height;
                            if (api.setTexture) {
                                api.setTexture(tex, b.normalTex, 0);
                            }
                            if (api.setAbsPoint) {
                                api.setAbsPoint(tex, 0, b.x - b.width, b.y + b.height, 1);
                                api.setAbsPoint(tex, 8, b.x, b.y, 1);
                            }
                            if (api.setVisible) {
                                api.setVisible(tex, b.enabled ? 1 : 0);
                            }
                            if (api.setTextureColor) {
                                uint32_t c = (b.actionId == 0 && b.active) ? 0xFF44FF3D : 0xFFFFFFFF;
                                api.setTextureColor(tex, &c);
                            }
                            b.texture = tex;
                        }
                    }
                }
            } __except (1) {
                b.button = nullptr;
                b.frame = nullptr;
                b.texture = nullptr;
            }
        }

        initialized = true;
        return true;
    }

    // Animate the visual region only. Native button host and logical hitbox
    // stay fixed, including when the pointer is on an icon's edge.
    void RefreshPressVisual(ToolbarButtonUI& b, ULONGLONG now) {
        if (!b.enabled || (b.pressed && now >= b.pressedUntil)) {
            b.pressed = false;
            b.pressStarted = b.pressedUntil = 0;
        }
        if (!b.texture) return;
        float amount = 0.0f;
        if (b.pressed) {
            const auto elapsed = now >= b.pressStarted ? now - b.pressStarted : 0;
            const float phase = std::min(1.0f, static_cast<float>(elapsed) / static_cast<float>(kPressAnimationMs));
            amount = (1.0f - phase) * (1.0f - phase); // Smooth return to the raised icon.
        }
        const float inset = 0.0010f * amount;
        const float dx = 0.0004f * amount, dy = -0.0004f * amount;
        const uint32_t base = b.actionId == 0 && b.active ? 0xff44ff3d : 0xffffffff;
        const float brightness = 1.0f - 0.40f * amount;
        uint32_t color = 0xff000000;
        for (int shift : {0, 8, 16}) {
            color |= static_cast<uint32_t>(((base >> shift) & 0xff) * brightness + 0.5f) << shift;
        }
        const char* path = b.pressed ? b.activeTex : b.normalTex;
        __try {
            if (api.setAbsPoint && b.enabled) {
                api.setAbsPoint(b.texture, 0, b.x - b.width + inset + dx, b.y + b.height - inset + dy, 1);
                api.setAbsPoint(b.texture, 8, b.x - inset + dx, b.y + inset + dy, 1);
            }
            if (api.setTexture && b.appliedTexture != path) {
                api.setTexture(b.texture, path, 0);
                b.appliedTexture = path;
            }
            if (api.setTextureColor && b.textureColor != color) {
                api.setTextureColor(b.texture, &color);
                b.textureColor = color;
            }
        } __except (1) {}
    }

    void UpdatePressAnimations(ULONGLONG now = GetTickCount64()) {
        for (auto& b : buttons) RefreshPressVisual(b, now);
    }

    void Update(uintptr_t game, uintptr_t parentHandle, const Config& config, bool isAutoPickOn) {
        if (!game || !parentHandle) {
            if (initialized) Reset();
            return;
        }

        if (buttons.size() >= 4) {
            buttons[0].enabled = config.showAutoPick;
            buttons[0].active = isAutoPickOn;
            buttons[1].enabled = config.showDropAll;
            buttons[2].enabled = config.showSellAll;
            buttons[3].enabled = config.showInsAllSkills;
        }

        if (!initialized || currentConsoleUI != parentHandle) {
            if (!Init(game, parentHandle)) return;
        }

        UpdatePressAnimations();

        for (auto& b : buttons) {
            void* target = b.button ? b.button : b.frame;
            if (!target && !b.texture) continue;
            __try {
                if (b.enabled) {
                    if (api.setAbsPoint) {
                        if (target) {
                            api.setAbsPoint(target, 0, b.x - b.width, b.y + b.height, 1);
                            api.setAbsPoint(target, 8, b.x, b.y, 1);
                        }
                    }
                    if (api.setVisible) {
                        if (target) api.setVisible(target, 1);
                        if (b.texture) api.setVisible(b.texture, 1);
                    }
                    if (b.button && api.setButtonState) {
                        api.setButtonState(b.button, 1);
                    }
                } else {
                    if (api.setAbsPoint) {
                        if (target) {
                            api.setAbsPoint(target, 0, 2.5f, 2.5f, 1);
                            api.setAbsPoint(target, 8, 2.5f + b.width, 2.5f - b.height, 1);
                        }
                        if (b.texture) {
                            api.setAbsPoint(b.texture, 0, 2.5f, 2.5f, 1);
                            api.setAbsPoint(b.texture, 8, 2.5f + b.width, 2.5f - b.height, 1);
                        }
                    }
                    if (api.setVisible) {
                        if (target) api.setVisible(target, 0);
                        if (b.texture) api.setVisible(b.texture, 0);
                    }
                    if (b.button && api.setButtonState) {
                        api.setButtonState(b.button, 0);
                    }
                }
            } __except (1) {}
        }
    }

    struct HeroContext {
        uint32_t handle = 0;
        uintptr_t unitPtr = 0;
        float x = 0.0f;
        float y = 0.0f;
        bool valid = false;
    };

    static inline bool HasAbility(uintptr_t game, uintptr_t unitPtr, uint32_t abilityId) {
        if (!game || !unitPtr || !abilityId || unitPtr < 0x10000 || unitPtr >= 0x7FFF0000) return false;
        __try {
            // 1. Direct ability array scan at unitPtr + 0x1DC / + 0x1E0
            for (uintptr_t offset : {0x1DCu, 0x1E0u}) {
                if (SafeIsReadable(reinterpret_cast<void*>(unitPtr + offset), 4)) {
                    uintptr_t abiList = *reinterpret_cast<uintptr_t*>(unitPtr + offset);
                    if (abiList >= 0x10000 && abiList < 0x7FFF0000 && SafeIsReadable(reinterpret_cast<void*>(abiList), 4 * 16)) {
                        for (int i = 0; i < 48; ++i) {
                            if (!SafeIsReadable(reinterpret_cast<void*>(abiList + i * 4), 4)) break;
                            uintptr_t abi = *reinterpret_cast<uintptr_t*>(abiList + i * 4);
                            if (abi >= 0x10000 && abi < 0x7FFF0000 && SafeIsReadable(reinterpret_cast<void*>(abi + 0x34), 4)) {
                                if (*reinterpret_cast<uint32_t*>(abi + 0x34) == abilityId) return true;
                            }
                        }
                    }
                }
            }

            // 2. Native UnitGetAbility call (ETS.mix 10072860 exact parity)
            uintptr_t fnGetAbility = game + ActiveGameOffsets().unitGetAbility;
            if (SafeIsExecutable(fnGetAbility)) {
                auto getAbi = reinterpret_cast<void*(__thiscall*)(void*, uint32_t, int, int, int, int)>(fnGetAbility);
                void* res = getAbi(reinterpret_cast<void*>(unitPtr), abilityId, 0, 1, 1, 1);
                if (res) return true;
            }

            // 3. Fallback: Direct ability linked list traversal
            uintptr_t node = 0;
            if (SafeIsReadable(reinterpret_cast<void*>(unitPtr + 0x1DC), 4)) {
                node = *reinterpret_cast<uintptr_t*>(unitPtr + 0x1DC);
            }
            if ((!node || node < 0x10000 || node >= 0x7FFF0000) && SafeIsReadable(reinterpret_cast<void*>(unitPtr + 0x1E0), 4)) {
                node = *reinterpret_cast<uintptr_t*>(unitPtr + 0x1E0);
            }
            for (int i = 0; i < 48 && node && node >= 0x10000 && node < 0x7FFF0000; ++i) {
                if (!SafeIsReadable(reinterpret_cast<void*>(node + 8), 4)) break;
                uintptr_t abi = *reinterpret_cast<uintptr_t*>(node + 8);
                if (abi && abi >= 0x10000 && abi < 0x7FFF0000 && SafeIsReadable(reinterpret_cast<void*>(abi + 0x34), 4)) {
                    if (*reinterpret_cast<uint32_t*>(abi + 0x34) == abilityId) return true;
                }
                if (!SafeIsReadable(reinterpret_cast<void*>(node), 4)) break;
                node = *reinterpret_cast<uintptr_t*>(node);
            }
        } __except (1) {}
        return false;
    }

    static inline bool IsShopUnit(uintptr_t game, uintptr_t uPtr) {
        if (!game || !uPtr) return false;
        static const uint32_t shopAbils[] = {
            0x41736569, // 'Asei' (Shop Sharing / Select Hero)
            0x4173656C, // 'Asel' (Shop Sharing)
            0x41706974, // 'Apit' (Purchase Item)
            0x41736964, // 'Asid' (Sell Item)
            0x416E6532, // 'Ane2' (Neutral Building 2)
            0x416E6575, // 'Aneu' (Neutral Building)
            0x41706F73, // 'Apos' (Powerup / Item Shop)
            0x41616D6B, // 'Aamk' (Goblin Merchant)
            0x41737564, // 'Asud' (Undead Shop)
            0x416D6169, // 'Amai' (Human Shop)
            0x4161676F, // 'Aago' (Orc Shop)
            0x416E6973  // 'Anis' (Night Elf Shop)
        };
        for (uint32_t abil : shopAbils) {
            if (HasAbility(game, uPtr, abil)) return true;
        }
        return false;
    }

    HeroContext ResolveLocalHero(uintptr_t game, uintptr_t root, void* (*jassLookup)(const char*), ItemManager& itemManager) {
        HeroContext res;
        if (!game) return res;

        auto extractCoords = [&](uint32_t h, uintptr_t u) {
            float x = 0.0f, y = 0.0f;
            auto fnGetUnitX = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(jassLookup ? jassLookup("GetUnitX") : LookupJassNative(game, "GetUnitX"));
            auto fnGetUnitY = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(jassLookup ? jassLookup("GetUnitY") : LookupJassNative(game, "GetUnitY"));
            if (fnGetUnitX && fnGetUnitY && h) {
                uint32_t xb = fnGetUnitX(h);
                uint32_t yb = fnGetUnitY(h);
                memcpy(&x, &xb, 4);
                memcpy(&y, &yb, 4);
            }
            if ((x == 0.0f && y == 0.0f) && u && itemManager.SafeIsReadable(reinterpret_cast<void*>(u + 0x288), 8)) {
                x = *reinterpret_cast<float*>(u + 0x288);
                y = *reinterpret_cast<float*>(u + 0x28C);
            }
            res.x = x;
            res.y = y;
        };

        // Method 0: ETS.mix exact memory path (100735c0 + 10073790) - ALWAYS PREFERRED FIRST
        __try {
            uintptr_t wRva = ActiveGameOffsets().world;
            if (itemManager.SafeIsReadable(reinterpret_cast<void*>(game + wRva), 4)) {
                uintptr_t pGlobal = *reinterpret_cast<uintptr_t*>(game + wRva);
                if (pGlobal && itemManager.SafeIsReadable(reinterpret_cast<void*>(pGlobal + 0x28), 2)) {
                    uint16_t playerIndex = *reinterpret_cast<uint16_t*>(pGlobal + 0x28);
                    if (playerIndex < 16 && itemManager.SafeIsReadable(reinterpret_cast<void*>(pGlobal + 0x58 + playerIndex * 4), 4)) {
                        uintptr_t p1 = *reinterpret_cast<uintptr_t*>(pGlobal + 0x58 + playerIndex * 4);
                        if (p1 && itemManager.SafeIsReadable(reinterpret_cast<void*>(p1 + 0x34), 4)) {
                            uintptr_t p2 = *reinterpret_cast<uintptr_t*>(p1 + 0x34);
                            if (p2 && itemManager.SafeIsReadable(reinterpret_cast<void*>(p2 + 0x1E0), 4)) {
                                uintptr_t heroUnitPtr = *reinterpret_cast<uintptr_t*>(p2 + 0x1E0);
                                if (heroUnitPtr && heroUnitPtr >= 0x10000) {
                                    uint32_t h = 0;
                                    if ((heroUnitPtr & 0xF0000000) != 0) {
                                        h = static_cast<uint32_t>(heroUnitPtr);
                                    } else {
                                        h = itemManager.GetUnitHandle(game, heroUnitPtr);
                                    }
                                    if (h) {
                                        res.handle = h;
                                        res.unitPtr = heroUnitPtr;
                                        extractCoords(h, heroUnitPtr);
                                        res.valid = true;
                                        return res;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } __except (1) {}

        // Method 1: Check UI selected unit in Selection Manager (root + 0x3bc)
        __try {
            if (root && itemManager.SafeIsReadable(reinterpret_cast<void*>(root + 0x3bc), 4)) {
                uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(root + 0x3bc);
                if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && itemManager.SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                    int count = *reinterpret_cast<int*>(selMgr + 0x28);
                    uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                    if (count > 0 && list && itemManager.SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                        uintptr_t selUnit = *reinterpret_cast<uintptr_t*>(list);
                        if (selUnit && itemManager.SafeIsReadable(reinterpret_cast<void*>(selUnit + 0x1f4), 4)) {
                            uintptr_t heroData = *reinterpret_cast<uintptr_t*>(selUnit + 0x1f4);
                            if (heroData) {
                                uint32_t h = itemManager.GetUnitHandle(game, selUnit);
                                if (h) {
                                    res.handle = h;
                                    res.unitPtr = selUnit;
                                    extractCoords(h, selUnit);
                                    res.valid = true;
                                    return res;
                                }
                            }
                        }
                    }
                }
            }
        } __except (1) {}

        // Method 2: JASS GroupEnumUnitsOfPlayer(grp, GetLocalPlayer(), 0)
        __try {
            auto resolveJass = [&](const char* n) { void* p = jassLookup ? jassLookup(n) : nullptr; return p ? p : LookupJassNative(game, n); };
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(resolveJass("GetLocalPlayer"));
            auto fnCreateGroup = reinterpret_cast<uint32_t(__cdecl*)()>(resolveJass("CreateGroup"));
            auto fnDestroyGroup = reinterpret_cast<void(__cdecl*)(uint32_t)>(resolveJass("DestroyGroup"));
            auto fnEnumPlayer = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t, uint32_t)>(resolveJass("GroupEnumUnitsOfPlayer"));
            auto fnFirstOfGroup = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("FirstOfGroup"));
            auto fnGroupRemove = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(resolveJass("GroupRemoveUnit"));
            auto fnGetUnitTypeId = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("GetUnitTypeId"));
            auto fnIsHeroUnitId = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("IsHeroUnitId"));

            if (fnLocal && fnCreateGroup && fnDestroyGroup && fnEnumPlayer && fnFirstOfGroup && fnGroupRemove) {
                uint32_t lp = fnLocal();
                if (lp) {
                    uint32_t grp = fnCreateGroup();
                    if (grp) {
                        fnEnumPlayer(grp, lp, 0);
                        uint32_t bestHandle = 0;
                        uintptr_t bestPtr = 0;
                        uint32_t fallbackHandle = 0;
                        uintptr_t fallbackPtr = 0;

                        while (true) {
                            uint32_t u = fnFirstOfGroup(grp);
                            if (!u) break;
                            fnGroupRemove(grp, u);

                            uintptr_t uPtr = 0;
                            uintptr_t htuRva = ActiveGameOffsets().handleToUnit;
                            if (itemManager.SafeIsExecutable(game + htuRva)) {
                                typedef uintptr_t (__thiscall *LookupUnit_t)(uint32_t handle);
                                auto lookupUnit = reinterpret_cast<LookupUnit_t>(game + htuRva);
                                uPtr = lookupUnit(u);
                            }

                            bool isHero = false;
                            if (fnIsHeroUnitId && fnGetUnitTypeId) {
                                uint32_t tid = fnGetUnitTypeId(u);
                                if (tid && fnIsHeroUnitId(tid)) isHero = true;
                            }
                            if (!isHero && uPtr && itemManager.SafeIsReadable(reinterpret_cast<void*>(uPtr + 0x1f4), 4)) {
                                if (*reinterpret_cast<uintptr_t*>(uPtr + 0x1f4) != 0) isHero = true;
                            }

                            if (isHero) {
                                float hp = 100.0f;
                                if (uPtr && itemManager.SafeIsReadable(reinterpret_cast<void*>(uPtr + 0x88), 4)) {
                                    hp = *reinterpret_cast<float*>(uPtr + 0x88);
                                }
                                if (hp > 0.405f) {
                                    bestHandle = u;
                                    bestPtr = uPtr;
                                    break;
                                } else if (!fallbackHandle) {
                                    fallbackHandle = u;
                                    fallbackPtr = uPtr;
                                }
                            }
                        }
                        fnDestroyGroup(grp);

                        uint32_t pickedHandle = bestHandle ? bestHandle : fallbackHandle;
                        uintptr_t pickedPtr = bestHandle ? bestPtr : fallbackPtr;
                        if (pickedHandle) {
                            res.handle = pickedHandle;
                            res.unitPtr = pickedPtr;
                            extractCoords(pickedHandle, pickedPtr);
                            res.valid = true;
                            return res;
                        }
                    }
                }
            }
        } __except (1) {}

        // Method 3: Fallback to itemManager.ResolveHeroUnit
        __try {
            uintptr_t hero = itemManager.ResolveHeroUnit(root, game);
            if (hero) {
                uint32_t h = itemManager.GetUnitHandle(game, hero);
                if (h) {
                    res.handle = h;
                    res.unitPtr = hero;
                    extractCoords(h, hero);
                    res.valid = true;
                    return res;
                }
            }
        } __except (1) {}

        // Method 4: Any selected unit in UI (for custom maps / couriers)
        __try {
            if (root && itemManager.SafeIsReadable(reinterpret_cast<void*>(root + 0x3bc), 4)) {
                uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(root + 0x3bc);
                if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && itemManager.SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                    int count = *reinterpret_cast<int*>(selMgr + 0x28);
                    uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                    if (count > 0 && list && itemManager.SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                        uintptr_t selUnit = *reinterpret_cast<uintptr_t*>(list);
                        if (selUnit) {
                            uint32_t h = itemManager.GetUnitHandle(game, selUnit);
                            if (h) {
                                res.handle = h;
                                res.unitPtr = selUnit;
                                extractCoords(h, selUnit);
                                res.valid = true;
                                return res;
                            }
                        }
                    }
                }
            }
        } __except (1) {}

        return res;
    }

    bool ExecuteDropAll(uintptr_t game, uintptr_t root, void* (*jassLookup)(const char*), ItemManager& itemManager) {
        if (!game) return false;
        __try {
            auto resolveJass = [&](const char* n) { void* p = jassLookup ? jassLookup(n) : nullptr; return p ? p : LookupJassNative(game, n); };
            auto fnUnitItemInSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(resolveJass("UnitItemInSlot"));
            auto fnUnitRemoveItemFromSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(resolveJass("UnitRemoveItemFromSlot"));
            auto fnUnitRemoveItem = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(resolveJass("UnitRemoveItem"));

            HeroContext target;
            // 1. Check UI selected unit (e.g. Courier or Hero)
            if (root && itemManager.SafeIsReadable(reinterpret_cast<void*>(root + 0x3bc), 4)) {
                uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(root + 0x3bc);
                if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && itemManager.SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                    int count = *reinterpret_cast<int*>(selMgr + 0x28);
                    uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                    if (count > 0 && list && itemManager.SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                        uintptr_t selUnit = *reinterpret_cast<uintptr_t*>(list);
                        if (selUnit) {
                            uint32_t h = itemManager.GetUnitHandle(game, selUnit);
                            if (h && fnUnitItemInSlot) {
                                bool selHasItem = false;
                                for (int s = 0; s < 6; ++s) {
                                    if (fnUnitItemInSlot(h, s)) { selHasItem = true; break; }
                                }
                                if (selHasItem) {
                                    target.handle = h;
                                    target.unitPtr = selUnit;
                                    target.valid = true;
                                }
                            }
                        }
                    }
                }
            }

            // 2. Fall back to local hero if selected unit has no items
            if (!target.valid) {
                target = ResolveLocalHero(game, root, jassLookup, itemManager);
            }

            if (!target.valid || !target.handle) return false;

            // Synchronously drop all items across all 6 slots over multiple passes
            int dropped = 0;
            for (int pass = 0; pass < 6; ++pass) {
                bool foundAny = false;
                for (int slot = 0; slot < 6; ++slot) {
                    __try {
                        uint32_t itm = fnUnitItemInSlot ? fnUnitItemInSlot(target.handle, slot) : 0;
                        if (itm || !fnUnitItemInSlot) {
                            if (fnUnitRemoveItemFromSlot) {
                                fnUnitRemoveItemFromSlot(target.handle, slot);
                                dropped++;
                                foundAny = true;
                            }
                            if (itm && fnUnitRemoveItem && fnUnitItemInSlot && fnUnitItemInSlot(target.handle, slot) != 0) {
                                fnUnitRemoveItem(target.handle, itm);
                                dropped++;
                                foundAny = true;
                            }
                        }
                    } __except (1) {}
                }
                if (!foundAny) break;
            }

            itemManager.lastDropAllCompleteTick = GetTickCount();
            itemManager.dropAllActive = false;

            return dropped > 0;
        } __except (1) {
            return false;
        }
    }

#pragma pack(push, 1)
    struct W3ActionDropItemOnTarget {
        uint8_t packetId;         // 0x13
        uint16_t flag;            // 0x0001
        uint32_t orderId;         // 0x000D0021 (852001: drop/sell item to target widget)
        uint32_t unk1;            // 0xFFFFFFFF
        uint32_t unk2;            // 0xFFFFFFFF
        float targetX;            // heroX
        float targetY;            // heroY
        uint32_t targetObjectId1; // *(uint32_t*)(shopPointer + 0x0C)
        uint32_t targetObjectId2; // *(uint32_t*)(shopPointer + 0x10)
        uint32_t itemObjectId1;   // *(uint32_t*)(itemPointer + 0x0C)
        uint32_t itemObjectId2;   // *(uint32_t*)(itemPointer + 0x10)
    };

    struct W3ActionPacketBuffer {
        uintptr_t vtable;  // game + 0x944E8C
        void* data;        // pointer to packet
        uint32_t unk1;     // 0
        uint32_t unk2;     // 0
        uint32_t size;     // sizeof(W3ActionDropItemOnTarget) = 39
        uint32_t unk3;     // 0xFFFFFFFF
    };
#pragma pack(pop)

    static inline uintptr_t GetObjectPointerFromHandle(uintptr_t game, uint32_t handle) {
        if (!game || !handle) return 0;
        const auto& o = ActiveGameOffsets();
        MEMORY_BASIC_INFORMATION m{};
        if (VirtualQuery(reinterpret_cast<void*>(game + o.actionPacketPlayer), &m, sizeof(m)) != sizeof(m) ||
            !(m.State == MEM_COMMIT && (m.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE))))
            return 0;
        auto fn = reinterpret_cast<uintptr_t(__fastcall*)(uint32_t, int)>(game + o.actionPacketPlayer);
        __try {
            return fn(handle, 0);
        } __except (1) {
            return 0;
        }
    }

    static inline bool SafeSendActionPacket(uintptr_t game, W3ActionPacketBuffer* buffer) {
        if (!game || !buffer) return false;
        const auto& o = ActiveGameOffsets();
        MEMORY_BASIC_INFORMATION m{};
        if (VirtualQuery(reinterpret_cast<void*>(game + o.transferSend), &m, sizeof(m)) != sizeof(m) ||
            !(m.State == MEM_COMMIT && (m.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE))))
            return false;
        auto sendPacket = reinterpret_cast<void(__fastcall*)(W3ActionPacketBuffer*, DWORD)>(game + o.transferSend);
        __try {
            sendPacket(buffer, 0);
            return true;
        } __except (1) {
            return false;
        }
    }

    bool ExecuteSellAll(uintptr_t game, uintptr_t /*root*/, void* (* /*jassLookup*/)(const char*), ItemManager& itemManager, std::string& outFeedback) {
        if(itemManager.sellAll.Active()) {
            outFeedback="|CFFFCD211Sell All Item|r: Already selling";
            return true;
        }
        if(itemManager.dropAllActive||itemManager.quickDropActive) {
            outFeedback="|CFFFCD211Sell All Item|r: Another inventory action is running";
            return false;
        }
        const auto result=itemManager.sellAll.Tick(game,GetTickCount(),true);
        itemManager.sellAllActive=itemManager.sellAll.Active();
        if(result==SellAll::Result::Working)outFeedback="|CFFFCD211Sell All Item|r: Finding nearby shop";
        else if(result==SellAll::Result::Empty)outFeedback="|CFFFCD211Sell All Item|r: Inventory is empty";
        else outFeedback="|CFFFCD211Sell All Item|r: Select your living hero with items";
        return result==SellAll::Result::Working||result==SellAll::Result::Empty;
    }
    static uintptr_t SkillPtr(uintptr_t at){return SafeIsReadable(reinterpret_cast<void*>(at),4)?*reinterpret_cast<uintptr_t*>(at):0;}
    static uintptr_t LocalSelectedSkillUnit(uintptr_t game){
        const auto world=SkillPtr(game+ActiveGameOffsets().world);
        if(!SafeIsReadable(reinterpret_cast<void*>(world+0x28),2))return 0;
        const unsigned slot=*reinterpret_cast<uint16_t*>(world+0x28);
        return slot<16?SkillPtr(SkillPtr(SkillPtr(world+0x58+slot*4)+0x34)+0x1e0):0;
    }
    static uint32_t SkillCode(uintptr_t button){return SkillPtr(SkillPtr(button+0x190)+4);}
    static uint32_t SkillOrder(uintptr_t button){return SkillPtr(SkillPtr(button+0x190)+8);}
    static bool SkillMenuOpen(uintptr_t (*resolve)(bool,int)){
        return resolve&&SkillOrder(resolve(false,11))==0xd0007;
    }
    // ETS 10061A30 -> 10052400: native UI clicks, never local JASS skill mutations.
    bool ExecuteIncreaseAllSkills(uintptr_t game, uintptr_t root, void* (*)(const char*),
                                  bool (*)(int), bool (*click)(bool,int,bool),
                                  uintptr_t (*resolve)(bool,int),ItemManager&) {
        if(!game||!root||!click||!resolve)return false;
        const auto unit=LocalSelectedSkillUnit(game);
        if(!unit||SkillPtr(SkillPtr(root+0x3f4)+0x238)!=unit)return false;
        const auto low=SkillPtr(unit+0xc),high=SkillPtr(unit+0x10);
        if(!SkillMenuOpen(resolve)){
            int learn=-1;
            for(int slot=0;slot<12;++slot){
                const auto button=resolve(false,slot);
                if(SkillCode(button)==0x41486572&&SkillOrder(button)==0xd0020){learn=slot;break;}
            }
            if(learn<0||!click(false,learn,false)||!SkillMenuOpen(resolve))return false;
        }
        bool submitted=false;
        for(int pass=0;pass<20;++pass){
            for(int index=0;index<11;++index){
                if(LocalSelectedSkillUnit(game)!=unit||SkillPtr(SkillPtr(root+0x3f4)+0x238)!=unit||
                   SkillPtr(unit+0xc)!=low||SkillPtr(unit+0x10)!=high)return submitted;
                if(!SkillMenuOpen(resolve))return submitted;
                // ETS visits native memory slots 0..10; Button() uses visual rows.
                const int slot=(index%3)*4+index/3;
                const auto button=resolve(false,slot),order=SkillOrder(button);
                if(SkillCode(button)==0x41486572&&order&&order!=0xd0020){
                    if(!click(false,slot,false))return submitted;
                    submitted=true;
                }
            }
        }
        if(LocalSelectedSkillUnit(game)==unit&&SkillPtr(unit+0xc)==low&&SkillPtr(unit+0x10)==high&&SkillMenuOpen(resolve))
            click(false,11,false); // native back/cancel, no simulated gameplay edits
        return submitted;
    }
    void SetButtonPressed(int actionId, bool pressed, ULONGLONG now = GetTickCount64()) {
        if (pressed && (actionId < 0 || actionId >= static_cast<int>(buttons.size()) ||
                        !buttons[actionId].enabled)) return;
        for (auto& b : buttons) {
            if (pressed) {
                b.pressed = b.actionId == actionId;
                b.pressStarted = b.pressed ? now : 0;
                b.pressedUntil = b.pressed ? now + kPressAnimationMs : 0;
            } else if (actionId >= 0 && b.actionId != actionId) {
                continue;
            }
            // Mouse-up/leave must not erase a quick tap before the next render.
            // The bounded pulse expires even if the release event is lost.
            RefreshPressVisual(b, now);
        }
    }

    void OnNativeClick(int actionId) {
        if (actionId < 0 || actionId >= static_cast<int>(buttons.size()) || !buttons[actionId].enabled) return;
        PlaySoundA("Sound\\Interface\\GlueScreenClick.wav", NULL, SND_ASYNC | SND_NODEFAULT);
        SetButtonPressed(actionId, true);
        if (onActionCallback) {
            onActionCallback(actionId);
        }
    }

    int GetActionAtPoint(POINT pt, int clientW, int clientH) const {
        if (clientW <= 0 || clientH <= 0) return -1;

        float uiX = (static_cast<float>(pt.x) / static_cast<float>(clientW)) * 0.8f;
        float uiY = (static_cast<float>(clientH - pt.y) / static_cast<float>(clientH)) * 0.6f;

        // Strictly hit-test within each button icon's exact bounding box (no outside padding or corridor)
        for (size_t i = 0; i < buttons.size(); ++i) {
            const auto& b = buttons[i];
            if (!b.enabled) continue;

            float left = b.x - b.width;
            float right = b.x;
            float bottom = b.y;
            float top = b.y + b.height;

            if (uiX >= left && uiX <= right && uiY >= bottom && uiY <= top) {
                return b.actionId;
            }
        }
        return -1;
    }

    bool IsPointInside(POINT pt, int clientW, int clientH) const {
        int action = GetActionAtPoint(pt, clientW, clientH);
        if (action < 0 || action >= static_cast<int>(buttons.size())) return false;
        return buttons[action].enabled;
    }

    bool ExecuteAction(int actionId, uintptr_t game, uintptr_t root,
                       void* (*jassLookup)(const char*),
                       bool (*clickDirectFn)(int),
                       bool (*clickBtnFn)(bool, int, bool),
                       uintptr_t (*buttonResolver)(bool, int),
                       ItemManager& itemManager,
                       GameplayCommandState& gameplayCommandState,
                       std::vector<std::string>& feedbackQueue,
                       const Config& /*config*/) {
        if (actionId < 0 || actionId >= static_cast<int>(buttons.size())) return false;
        if (!buttons[actionId].enabled) return false;

        switch (actionId) {
            case 0: { // Auto Pick Item
                gameplayCommandState.autoPick = !gameplayCommandState.autoPick;
                itemManager.autoPickEnabled = gameplayCommandState.autoPick;
                if (buttons.size() > 0) {
                    buttons[0].active = gameplayCommandState.autoPick;
                    RefreshPressVisual(buttons[0], GetTickCount64());
                }
                if (gameplayCommandState.autoPick) {
                    feedbackQueue.push_back("|CFFFCD211Auto Pick Item|r: |cff44ff3dOn|r");
                } else {
                    feedbackQueue.push_back("|CFFFCD211Auto Pick Item|r: |cffff5c5cOff|r");
                }
                return true;
            }
            case 1: { // Drop All Item
                feedbackQueue.push_back("|CFFFCD211Drop Item All|r");
                ExecuteDropAll(game, root, jassLookup, itemManager);
                return true;
            }
            case 2: { // Sell All Item
                std::string fb;
                ExecuteSellAll(game, root, jassLookup, itemManager, fb);
                feedbackQueue.push_back(fb);
                return true;
            }
            case 3: { // Increase All Skills
                feedbackQueue.push_back("|CFFFCD211Increase All Skills|r");
                ExecuteIncreaseAllSkills(game, root, jassLookup, clickDirectFn, clickBtnFn, buttonResolver, itemManager);
                return true;
            }
            default:
                break;
        }
        return false;
    }

    bool HandleClick(POINT pt, int clientW, int clientH, uintptr_t game, uintptr_t root,
                    void* (*jassLookup)(const char*),
                    bool (*clickDirectFn)(int),
                    bool (*clickBtnFn)(bool, int, bool),
                    uintptr_t (*buttonResolver)(bool, int),
                    ItemManager& itemManager,
                    GameplayCommandState& gameplayCommandState,
                    std::vector<std::string>& feedbackQueue,
                    const Config& config) {
        if (clientW <= 0 || clientH <= 0 || !root || !game) return false;

        int clickedAction = GetActionAtPoint(pt, clientW, clientH);
        if (clickedAction < 0 || clickedAction >= static_cast<int>(buttons.size())) return false;
        if (!buttons[clickedAction].enabled) return false;

        SetButtonPressed(clickedAction, true);
        DWORD now = GetTickCount();
        if (clickedAction == lastAction && (now - lastClickTick) < 250) {
            return true;
        }
        lastClickTick = now;
        lastAction = clickedAction;

        PlaySoundA("Sound\\Interface\\GlueScreenClick.wav", NULL, SND_ASYNC | SND_NODEFAULT);

        return ExecuteAction(clickedAction, game, root, jassLookup,
                             clickDirectFn, clickBtnFn, buttonResolver,
                             itemManager, gameplayCommandState, feedbackQueue, config);
    }
};

inline void __thiscall ToolbarObserver::OnFrameEvent(void* eventObj) {
    if (!toolbar) return;
    uint32_t eventId = 0;
    if (eventObj) {
        eventId = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(eventObj) + 8);
    }
    if (eventId == 3000) { // Native Button Click
        toolbar->OnNativeClick(actionId);
    } else if (eventId == 3002) { // Mouse Leave
        toolbar->SetButtonPressed(actionId, false);
    }
}

} // namespace wht

