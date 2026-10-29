#pragma once
#include "network_ping.hpp"
#include <string>
#include <sstream>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <vector>
#include "common.hpp"
#include "resource_transfer.hpp"
#include "item_manager.hpp"
#include "courier_mth_manager.hpp"

namespace wht {

namespace detail {
    inline uint32_t ParseRawcode(const std::wstring& s) {
        if (s.empty()) return 0;
        if (s.length() >= 4 && s[0] == L'0' && (s[1] == L'x' || s[1] == L'X')) {
            return static_cast<uint32_t>(wcstoul(s.c_str(), nullptr, 16));
        }
        if (s.length() >= 4) {
            return (static_cast<uint32_t>(static_cast<uint8_t>(s[0])) << 24) |
                   (static_cast<uint32_t>(static_cast<uint8_t>(s[1])) << 16) |
                   (static_cast<uint32_t>(static_cast<uint8_t>(s[2])) << 8) |
                   (static_cast<uint32_t>(static_cast<uint8_t>(s[3])));
        }
        return 0;
    }

    inline bool TrySetInvulnerable(uintptr_t selUnit, bool invul) {
        __try {
            if (!selUnit) return false;
            if (invul) {
                float maxHp = *reinterpret_cast<float*>(selUnit + 0x8c);
                if (maxHp > 0.0f) *reinterpret_cast<float*>(selUnit + 0x88) = maxHp;
                *reinterpret_cast<uint32_t*>(selUnit + 0x20) |= 0x1000;
            } else {
                *reinterpret_cast<uint32_t*>(selUnit + 0x20) &= ~0x1000;
            }
            return true;
        } __except (1) {
            return false;
        }
    }

    inline bool TryKillUnit(uintptr_t selUnit) {
        __try {
            if (!selUnit) return false;
            *reinterpret_cast<float*>(selUnit + 0x88) = 0.0f;
            return true;
        } __except (1) {
            return false;
        }
    }

    inline bool SafeIsReadable(const void* p, size_t n) {
        MEMORY_BASIC_INFORMATION m{};
        if (!p || !n || VirtualQuery(p, &m, sizeof(m)) != sizeof(m) || m.State != MEM_COMMIT || (m.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
        uintptr_t start = reinterpret_cast<uintptr_t>(p), end = reinterpret_cast<uintptr_t>(m.BaseAddress) + m.RegionSize;
        return start <= end && n <= end - start;
    }

    inline bool SafeIsExecutable(uintptr_t at) {
        MEMORY_BASIC_INFORMATION m{};
        return at && VirtualQuery(reinterpret_cast<void*>(at), &m, sizeof(m)) == sizeof(m) && m.State == MEM_COMMIT &&
            !(m.Protect & (PAGE_GUARD | PAGE_NOACCESS)) && (m.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
    }

    inline uint32_t TryGetUnitHandle(uintptr_t game, uintptr_t unit) {
        if (!unit || unit < 0x10000 || unit >= 0x7FFF0000) return 0;
        if (!SafeIsReadable(reinterpret_cast<void*>(unit), 0x100)) return 0;
        if (game) {
            uintptr_t vtbl = *reinterpret_cast<const uintptr_t*>(unit);
            if (vtbl < game || vtbl >= game + 0xC00000) return 0;
        }
        __try {
            uintptr_t umRva = ActiveGameOffsets().getUnitManager;
            uintptr_t luRva = ActiveGameOffsets().lookupUnit;
            uintptr_t wRva = ActiveGameOffsets().world;
            if (game && SafeIsExecutable(game + umRva) && SafeIsExecutable(game + luRva) && SafeIsReadable(reinterpret_cast<void*>(game + wRva), 4)) {
                uintptr_t state = *reinterpret_cast<uintptr_t*>(game + wRva);
                if (state && SafeIsReadable(reinterpret_cast<void*>(state), 0x20)) {
                    auto getContext = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(game + umRva);
                    uintptr_t ctx = getContext(state);
                    if (ctx && SafeIsReadable(reinterpret_cast<void*>(ctx), 0x20)) {
                        auto getHandle = reinterpret_cast<uint32_t(__thiscall*)(uintptr_t, uintptr_t, int)>(game + luRva);
                        uint32_t h = getHandle(ctx, unit, 0);
                        if (h) return h;
                    }
                }
            }
            if (SafeIsReadable(reinterpret_cast<void*>(unit), 0x20)) {
                uint32_t h = *reinterpret_cast<uint32_t*>(unit + 0x1c);
                if (h >= 0x100000 && h < 0x20000000) return h;
                h = *reinterpret_cast<uint32_t*>(unit + 0x18);
                if (h >= 0x100000 && h < 0x20000000) return h;
            }
            return 0;
        } __except (1) {
            return 0;
        }
    }
    inline uint32_t TryGetUnitHandle(uintptr_t unit) {
        return TryGetUnitHandle(0, unit);
    }

    inline bool TrySetHeroLevel(uintptr_t game, void* (*jassLookup)(const char*), uintptr_t selUnit, int lvl, bool& notHero) {
        notHero = false;
        if (!selUnit) return false;
        bool memoryUpdated = false;
        __try {
            uintptr_t heroData = *reinterpret_cast<uintptr_t*>(selUnit + 0x1f4);
            if (heroData && heroData >= 0x10000 && heroData < 0x7FFF0000) {
                *reinterpret_cast<int*>(heroData + 0x70) = lvl;
                memoryUpdated = true;
            } else {
                notHero = true;
            }
        } __except (1) {}

        if (game && jassLookup) {
            uint32_t h = TryGetUnitHandle(game, selUnit);
            if (h) {
                auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, int, uint32_t)>(jassLookup("SetHeroLevel"));
                if (fn && SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) {
                    __try {
                        fn(h, lvl, 1);
                    } __except (1) {}
                }
            }
        }
        return memoryUpdated;
    }
    inline bool TrySetHeroLevel(uintptr_t selUnit, int lvl, bool& notHero) {
        return TrySetHeroLevel(0, nullptr, selUnit, lvl, notHero);
    }

    inline bool TrySetHeroStats(uintptr_t game, void* (*jassLookup)(const char*), uintptr_t selUnit, int statType, int val, bool& notHero) {
        notHero = false;
        if (!selUnit) return false;
        bool memoryUpdated = false;
        __try {
            uintptr_t heroData = *reinterpret_cast<uintptr_t*>(selUnit + 0x1f4);
            if (heroData && heroData >= 0x10000 && heroData < 0x7FFF0000) {
                if (statType == 0) *reinterpret_cast<float*>(heroData + 0x88) = static_cast<float>(val); // STR
                else if (statType == 1) *reinterpret_cast<float*>(heroData + 0x90) = static_cast<float>(val); // AGI
                else if (statType == 2) *reinterpret_cast<float*>(heroData + 0x98) = static_cast<float>(val); // INT
                else if (statType == 3) { // ALL
                    *reinterpret_cast<float*>(heroData + 0x88) = static_cast<float>(val);
                    *reinterpret_cast<float*>(heroData + 0x90) = static_cast<float>(val);
                    *reinterpret_cast<float*>(heroData + 0x98) = static_cast<float>(val);
                } else if (statType == 4) { // SP
                    *reinterpret_cast<int*>(heroData + 0x78) += val;
                }
                memoryUpdated = true;
            } else {
                notHero = true;
            }
        } __except (1) {}

        if (game && jassLookup) {
            uint32_t h = TryGetUnitHandle(game, selUnit);
            if (h) {
                if (statType == 0 || statType == 3) {
                    auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, int, uint32_t)>(jassLookup("SetHeroStr"));
                    if (fn && SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) { __try { fn(h, val, 1); } __except (1) {} }
                }
                if (statType == 1 || statType == 3) {
                    auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, int, uint32_t)>(jassLookup("SetHeroAgi"));
                    if (fn && SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) { __try { fn(h, val, 1); } __except (1) {} }
                }
                if (statType == 2 || statType == 3) {
                    auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, int, uint32_t)>(jassLookup("SetHeroInt"));
                    if (fn && SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) { __try { fn(h, val, 1); } __except (1) {} }
                }
            }
        }
        return memoryUpdated;
    }
    inline bool TrySetHeroStats(uintptr_t selUnit, int statType, int val, bool& notHero) {
        return TrySetHeroStats(0, nullptr, selUnit, statType, val, notHero);
    }

    inline bool TryGetCursorPosition(uintptr_t game, float& x, float& y) {
        __try {
            if (!game) return false;
            uintptr_t pMgr = *reinterpret_cast<uintptr_t*>(game + ActiveGameOffsets().cursorManager);
            if (!pMgr || pMgr < 0x10000 || pMgr >= 0x7FFF0000) return false;
            uintptr_t pWorld = *reinterpret_cast<uintptr_t*>(pMgr + 0x3bc);
            if (!pWorld || pWorld < 0x10000 || pWorld >= 0x7FFF0000) return false;
            float worldX = *reinterpret_cast<float*>(pWorld + 0x310);
            float worldY = *reinterpret_cast<float*>(pWorld + 0x314);
            if (std::isfinite(worldX) && std::isfinite(worldY) &&
                std::abs(worldX) < 100000.0f && std::abs(worldY) < 100000.0f) {
                x = worldX;
                y = worldY;
                return true;
            }
            return false;
        } __except (1) {
            return false;
        }
    }

    inline bool TryTeleportUnit(uintptr_t game, void* (*jassLookup)(const char*), uintptr_t selUnit, bool& noCoords) {
        noCoords = false;
        if (!game || !selUnit) return false;
        float worldX = 0.0f, worldY = 0.0f;
        if (TryGetCursorPosition(game, worldX, worldY)) {
            __try {
                *reinterpret_cast<float*>(selUnit + 0x288) = worldX;
                *reinterpret_cast<float*>(selUnit + 0x28c) = worldY;
            } __except (1) {
                return false;
            }
            if (jassLookup) {
                uint32_t h = TryGetUnitHandle(game, selUnit);
                if (h) {
                    auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, const float*, const float*)>(jassLookup("SetUnitPosition"));
                    if (fn && SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) {
                        __try {
                            fn(h, &worldX, &worldY);
                        } __except (1) {}
                    }
                }
            }
            return true;
        }
        noCoords = true;
        return false;
    }
    inline bool TryTeleportUnit(uintptr_t game, uintptr_t selUnit, bool& noCoords) {
        return TryTeleportUnit(game, nullptr, selUnit, noCoords);
    }

    inline uint32_t TryGetUnitTypeId(uintptr_t unit) {
        __try {
            if (!unit) return 0;
            return *reinterpret_cast<uint32_t*>(unit + 0x30);
        } __except (1) {
            return 0;
        }
    }

    inline bool TryGetUnitPosition(uintptr_t unit, float& x, float& y) {
        __try {
            if (!unit) return false;
            x = *reinterpret_cast<float*>(unit + 0x288);
            y = *reinterpret_cast<float*>(unit + 0x28c);
            return std::isfinite(x) && std::isfinite(y);
        } __except (1) {
            return false;
        }
    }

    inline bool TryDamageUnit(uintptr_t unit, float dmg) {
        __try {
            if (!unit) return false;
            float* curHp = reinterpret_cast<float*>(unit + 0x88);
            *curHp = std::max(0.0f, *curHp - dmg);
            return true;
        } __except (1) {
            return false;
        }
    }

    inline bool TrySetBaseDamage(uintptr_t unit, float dmg) {
        __try {
            if (!unit) return false;
            *reinterpret_cast<float*>(unit + 0xac) = dmg;
            return true;
        } __except (1) {
            return false;
        }
    }

    inline bool TryDeleteUnit(uintptr_t unit) {
        __try {
            if (!unit) return false;
            *reinterpret_cast<float*>(unit + 0x88) = 0.0f;
            *reinterpret_cast<uint32_t*>(unit + 0x20) |= 0x1;
            return true;
        } __except (1) {
            return false;
        }
    }

    inline uint32_t SafeCreateUnit(
        void* (*jassLookup)(const char*),
        uint32_t player, int unitid, float x, float y, float face)
    {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int, const float*, const float*, const float*)>(jassLookup("CreateUnit"));
        if (!fn || !player || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(player, unitid, &x, &y, &face);
        } __except (1) {
            return 0;
        }
    }

    inline uint32_t SafeCreateItem(
        void* (*jassLookup)(const char*),
        int itemid, float x, float y)
    {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(int, const float*, const float*)>(jassLookup("CreateItem"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(itemid, &x, &y);
        } __except (1) {
            return 0;
        }
    }

    inline void SafeRemoveUnit(void* (*jassLookup)(const char*), uint32_t h) {
        auto fn = reinterpret_cast<void(__cdecl*)(uint32_t)>(jassLookup("RemoveUnit"));
        if (!fn || !h || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(h);
        } __except (1) {}
    }

    inline uint32_t SafeAddAbility(void* (*jassLookup)(const char*), uint32_t h, int abi) {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(jassLookup("UnitAddAbility"));
        if (!fn || !h || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(h, abi);
        } __except (1) {
            return 0;
        }
    }

    inline uint32_t SafeRemoveAbility(void* (*jassLookup)(const char*), uint32_t h, int abi) {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(jassLookup("UnitRemoveAbility"));
        if (!fn || !h || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(h, abi);
        } __except (1) {
            return 0;
        }
    }

    inline int SafeSetAbilityLevel(void* (*jassLookup)(const char*), uint32_t h, int abi, int lvl) {
        auto fn = reinterpret_cast<int(__cdecl*)(uint32_t, int, int)>(jassLookup("SetUnitAbilityLevel"));
        if (!fn || !h || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(h, abi, lvl);
        } __except (1) {
            return 0;
        }
    }

    inline std::wstring RawcodeToWString(uint32_t code) {
        if (!code) return L"none";
        wchar_t buf[5];
        buf[0] = static_cast<wchar_t>((code >> 24) & 0xFF);
        buf[1] = static_cast<wchar_t>((code >> 16) & 0xFF);
        buf[2] = static_cast<wchar_t>((code >> 8) & 0xFF);
        buf[3] = static_cast<wchar_t>(code & 0xFF);
        buf[4] = L'\0';
        return std::wstring(buf);
    }

    inline void SafeSetUnitScale(void* (*jassLookup)(const char*), uint32_t h, float sx, float sy, float sz) {
        auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, const float*, const float*, const float*)>(jassLookup("SetUnitScale"));
        if (!fn || !h || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(h, &sx, &sy, &sz);
        } __except (1) {}
    }

    inline void SafePanCameraTo(void* (*jassLookup)(const char*), float x, float y) {
        auto fn = reinterpret_cast<void(__cdecl*)(const float*, const float*)>(jassLookup("PanCameraTo"));
        if (!fn) fn = reinterpret_cast<void(__cdecl*)(const float*, const float*)>(jassLookup("SetCameraPosition"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(&x, &y);
        } __except (1) {}
    }

    inline uint32_t SafeUnitItemInSlot(void* (*jassLookup)(const char*), uint32_t h, int slot) {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(jassLookup("UnitItemInSlot"));
        if (!fn || !h || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(h, slot);
        } __except (1) {
            return 0;
        }
    }

    inline uint32_t SafeGetItemTypeId(void* (*jassLookup)(const char*), uint32_t itemHandle) {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(jassLookup("GetItemTypeId"));
        if (!fn || !itemHandle || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(itemHandle);
        } __except (1) {
            return 0;
        }
    }

    inline void SafeSelectUnit(void* (*jassLookup)(const char*), uint32_t h, bool flag) {
        auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(jassLookup("SelectUnit"));
        if (!fn || !h || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(h, flag ? 1 : 0);
        } __except (1) {}
    }

    inline void SafeClearSelection(void* (*jassLookup)(const char*)) {
        auto fn = reinterpret_cast<void(__cdecl*)()>(jassLookup("ClearSelection"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn();
        } __except (1) {}
    }

    inline bool TryModifyAbilityButton(uintptr_t selUnit, uint32_t abiCode, int delta) {
        __try {
            if (!selUnit) return false;
            uintptr_t abiList = *reinterpret_cast<uintptr_t*>(selUnit + 0x1dc);
            if (!abiList || abiList < 0x10000 || abiList >= 0x7FFF0000) return false;
            for (int i = 0; i < 32; ++i) {
                uintptr_t abi = *reinterpret_cast<uintptr_t*>(abiList + i * 4);
                if (abi >= 0x10000 && abi < 0x7FFF0000) {
                    uint32_t code = *reinterpret_cast<uint32_t*>(abi + 0x34);
                    if (code == abiCode) {
                        *reinterpret_cast<int*>(abi + 0x40) += delta;
                        return true;
                    }
                }
            }
            return false;
        } __except (1) {
            return false;
        }
    }

    inline uint32_t TryGetUnitFlags(uintptr_t selUnit) {
        __try {
            if (!selUnit) return 0;
            return *reinterpret_cast<uint32_t*>(selUnit + 0x20);
        } __except (1) {
            return 0;
        }
    }

    inline void SafeSetGameSpeed(void* (*jassLookup)(const char*), int speed) {
        auto fn = reinterpret_cast<void(__cdecl*)(int)>(jassLookup("SetGameSpeed"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(speed);
        } __except (1) {}
    }

    inline void SafeSetCameraField(void* (*jassLookup)(const char*), int field, float val, float duration) {
        auto fn = reinterpret_cast<void(__cdecl*)(int, const float*, const float*)>(jassLookup("SetCameraField"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(field, &val, &duration);
        } __except (1) {}
    }

    inline void SafePingMinimap(void* (*jassLookup)(const char*), float x, float y, float duration) {
        auto fn = reinterpret_cast<void(__cdecl*)(const float*, const float*, const float*)>(jassLookup("PingMinimap"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(&x, &y, &duration);
        } __except (1) {}
    }

    inline uint32_t SafePlayer(void* (*jassLookup)(const char*), int slot) {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(int)>(jassLookup("Player"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(slot);
        } __except (1) {
            return 0;
        }
    }

    inline std::string Utf8Narrow(const std::wstring& s) {
        int n = WideCharToMultiByte(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0, nullptr, nullptr);
        std::string out(n, '\0');
        if (n) WideCharToMultiByte(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), out.data(), n, nullptr, nullptr);
        return out;
    }

    inline bool TryGetUnitModel(uintptr_t selUnit, char* outBuf, size_t bufSize) {
        __try {
            if (!selUnit || !outBuf || bufSize == 0) return false;
            uintptr_t candidates[] = { selUnit, *reinterpret_cast<uintptr_t*>(selUnit + 0x28) };
            for (uintptr_t u : candidates) {
                if (!u || u < 0x10000 || u >= 0x7FFF0000) continue;
                uintptr_t typeInfo = *reinterpret_cast<uintptr_t*>(u + 0x60);
                if (typeInfo && typeInfo >= 0x10000 && typeInfo < 0x7FFF0000) {
                    for (int off : { 0x1c, 0x20, 0x24, 0x28, 0x2c }) {
                        const char* model = *reinterpret_cast<const char**>(typeInfo + off);
                        if (model && reinterpret_cast<uintptr_t>(model) >= 0x10000 && reinterpret_cast<uintptr_t>(model) < 0x7FFF0000) {
                            if (strstr(model, ".md") != nullptr || strstr(model, "\\") != nullptr) {
                                strncpy_s(outBuf, bufSize, model, _TRUNCATE);
                                return true;
                            }
                        }
                    }
                }
            }
            uint32_t tid = *reinterpret_cast<uint32_t*>(selUnit + 0x30);
            if (tid) {
                char code[5] = {
                    static_cast<char>((tid >> 24) & 0xff),
                    static_cast<char>((tid >> 16) & 0xff),
                    static_cast<char>((tid >> 8) & 0xff),
                    static_cast<char>(tid & 0xff),
                    0
                };
                sprintf_s(outBuf, bufSize, "units\\%s.mdx", code);
                return true;
            }
            return false;
        } __except (1) {
            return false;
        }
    }

    inline void SafeSetPlayerAlliance(void* (*jassLookup)(const char*), uint32_t p1, uint32_t p2, int setting, bool val) {
        auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t, int, uint32_t)>(jassLookup("SetPlayerAlliance"));
        if (!fn || !p1 || !p2 || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(p1, p2, setting, val ? 1 : 0);
        } __except (1) {}
    }

    inline bool SafeSaveGame(void* (*jassLookup)(const char*), const char* name) {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(const char*)>(jassLookup("SaveGame"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return false;
        __try {
            return fn(name) != 0;
        } __except (1) {
            return false;
        }
    }

    inline void SafeSetUnitMoveSpeed(void* (*jassLookup)(const char*), uint32_t h, float speed) {
        auto fn = reinterpret_cast<void(__cdecl*)(uint32_t, const float*)>(jassLookup("SetUnitMoveSpeed"));
        if (!fn || !h || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(h, &speed);
        } __except (1) {}
    }

    inline bool TrySetUnitAttackSpeed(uintptr_t selUnit, float factor) {
        __try {
            if (!selUnit || !SafeIsReadable(reinterpret_cast<void*>(selUnit + 0x1B0), sizeof(float))) return false;
            *reinterpret_cast<float*>(selUnit + 0x1B0) = factor;
            return true;
        } __except (1) {
            return false;
        }
    }

    inline uint32_t SafeGetWorldBounds(void* (*jassLookup)(const char*)) {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetWorldBounds"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn();
        } __except (1) {
            return 0;
        }
    }

    inline uint32_t SafeCreateFogModifier(void* (*jassLookup)(const char*), uint32_t player, int state, uint32_t rect, bool useSharedVision, bool afterShared) {
        auto fn = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int, uint32_t, uint32_t, uint32_t)>(jassLookup("CreateFogModifierRect"));
        if (!fn || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return 0;
        __try {
            return fn(player, state, rect, useSharedVision ? 1 : 0, afterShared ? 1 : 0);
        } __except (1) {
            return 0;
        }
    }

    inline void SafeFogModifierStart(void* (*jassLookup)(const char*), uint32_t fogMod) {
        auto fn = reinterpret_cast<void(__cdecl*)(uint32_t)>(jassLookup("FogModifierStart"));
        if (!fn || !fogMod || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(fogMod);
        } __except (1) {}
    }

    inline void SafeFogModifierStop(void* (*jassLookup)(const char*), uint32_t fogMod) {
        auto fn = reinterpret_cast<void(__cdecl*)(uint32_t)>(jassLookup("FogModifierStop"));
        if (!fn || !fogMod || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(fogMod);
        } __except (1) {}
    }

    inline void SafeDestroyFogModifier(void* (*jassLookup)(const char*), uint32_t fogMod) {
        auto fn = reinterpret_cast<void(__cdecl*)(uint32_t)>(jassLookup("DestroyFogModifier"));
        if (!fn || !fogMod || !SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))) return;
        __try {
            fn(fogMod);
        } __except (1) {}
    }
} // namespace detail

struct GameplayCommandState {
    bool noCd = false;
    uint32_t lastNoCdTick = 0;
    float gameSpeed = 1.0f;
    bool widescreen = false;
    bool cursorLocked = false;
    bool menuVisible = false;
    std::vector<int> mutedSlots;
    std::atomic<uint32_t> mutedMask{0};
    bool IsMuted(uint32_t sender) const {
        return sender < 16 && (mutedMask.load(std::memory_order_relaxed) & (1u << sender)) != 0;
    }
    float presetCamDist = 2400.0f;
    int repeatSlot = -1;
    bool repeatAll = false;
    int spamSlot = -1;
    bool spamAll = false;
    uint64_t nextSpam = 0;
    std::wstring lastChatMsg;
    bool viewChat = false;
    std::wstring customModelPath;
    std::vector<std::string> pendingChatMessages;
    uint64_t nextRepeat=0;
    std::string repeatText;
    uint32_t repeatRemaining=0;
    template<class Send> bool ProcessRepeat(uint64_t now,Send send){
        if(!repeatRemaining||now<nextRepeat||!send(repeatText))return false;
        --repeatRemaining;nextRepeat=now+250;
        if(!repeatRemaining)repeatText.clear();
        return true;
    }
    void ObserveChat(uint32_t sender,uint32_t local,const char* text){
        if(sender>=16||sender==local||!text||!*text||*text=='#'||
           (!repeatAll&&repeatSlot!=static_cast<int>(sender)))return;
        const auto length=strnlen_s(text,129);
        if(length>128||pendingChatMessages.size()>=64)return;
        pendingChatMessages.emplace_back(text,length);
    }
    bool autoPick = false;
    bool autoPickInitialized = false;
    bool quickDrop = true;
    bool autoClickLeft = false;
    bool autoClickRight = false;
    unsigned autoClickInterval = 10;
    uint32_t activeFogModifier = 0;

    void Reset() {
        noCd = false;
        lastNoCdTick = 0;
        gameSpeed = 1.0f;
        widescreen = false;
        cursorLocked = false;
        menuVisible = false;
        mutedSlots.clear();
        mutedMask.store(0, std::memory_order_relaxed);
        presetCamDist = 2400.0f;
        repeatSlot = -1;
        repeatAll=false;spamSlot=-1;spamAll=false;nextSpam=0;
        lastChatMsg.clear();
        viewChat = false;
        customModelPath.clear();
        pendingChatMessages.clear();
        nextRepeat=0;
        repeatText.clear();repeatRemaining=0;
        quickDrop = true;
        autoClickLeft = false;
        autoClickRight = false;
        activeFogModifier = 0;
    }

    CourierMthManager* courierMgr = nullptr;

    bool IsGameplayCommand(const std::wstring& name) const {
        return name == L"#gold" || name == L"#lumber" || name == L"#food" || name == L"#fullres" ||
               name == L"#nocd" || name == L"#yescd" || name == L"#invul" || name == L"#vul" ||
               name == L"#kill" || name == L"#lvl" || name == L"#fullatt" || name == L"#str" ||
               name == L"#agi" || name == L"#int" || name == L"#sp" || name == L"#tele" ||
               name == L"#create" || name == L"#copy" || name == L"#udel" || name == L"#udmg" ||
               name == L"#udmgb" || name == L"#sabi" || name == L"#rabi" || name == L"#addall" ||
               name == L"#raddall" || name == L"#size" || name == L"#upos" || name == L"#mpos" ||
               name == L"#ppos" || name == L"#cname" || name == L"#islot" || name == L"#uflag" ||
               name == L"#wsize" || name == L"#hideb" || name == L"#showb" || name == L"#hideball" ||
               name == L"#showball" || name == L"#ufind" || name == L"#ufinds" || name == L"#selc" ||
               name == L"#speed" || name == L"#wide" || name == L"#lock" || name == L"#menu" ||
               name == L"#ping" || name == L"#mute" || name == L"#unmute" || name == L"#ggl" ||
               name == L"#gll" || name == L"#pcam" || name == L"#pzoom" ||
               name == L"#nhailai" || name == L"#mtrade" || name == L"#gmodel" || name == L"#emodel" ||
               name == L"#smodel" || name == L"#spam" || name == L"#repeat" || name == L"#shareall" || name == L"#spamall" ||
               name == L"#save" || name == L"#srand" || name == L"#findCP" ||
               name == L"#autopick" || name == L"#dropall" || name == L"#drop" || name == L"#quickdrop" ||
               name == L"#autol" || name == L"#autor" || name == L"#autoclickl" || name == L"#autoclickr" || name == L"#clickdelay" ||
               name == L"#ms" || name == L"#as" || name == L"#mh" ||
               name == L"#ga" || name == L"#courier" || name == L"#chuyendo" ||
               name == L"#chayga" || name == L"#chaysoi" || name == L"#soi" || name == L"#caysoi" ||
               name == L"#setga" || name == L"#backga" || name == L"#backsoi" || name == L"#gabase" ||
               name == L"#stopga" || name == L"#stopmth";
    }

    uintptr_t GetSelectedUnit(uintptr_t root) {
        if (!root) return 0;
        __try {
            uintptr_t panel = *reinterpret_cast<uintptr_t*>(root + 0x3f4);
            if (!panel) return 0;
            return *reinterpret_cast<uintptr_t*>(panel + 0x238);
        } __except (1) {
            return 0;
        }
    }

    bool ResetUnitCooldowns(uintptr_t unit) {
        if (!unit) return false;
        __try {
            uintptr_t abilityList = *reinterpret_cast<uintptr_t*>(unit + 0x1dc);
            if (!abilityList || abilityList < 0x10000 || abilityList >= 0x7FFF0000) {
                abilityList = *reinterpret_cast<uintptr_t*>(unit + 0x1e0);
            }
            if (abilityList && abilityList >= 0x10000 && abilityList < 0x7FFF0000) {
                uintptr_t head = abilityList;
                for (int i = 0; i < 32 && head && head >= 0x10000 && head < 0x7FFF0000; ++i) {
                    uintptr_t abi = *reinterpret_cast<uintptr_t*>(head + 8);
                    if (abi && abi >= 0x10000 && abi < 0x7FFF0000) {
                        float* cd1 = reinterpret_cast<float*>(abi + 0x68);
                        if (*cd1 > 0.0f && *cd1 < 10000.0f) *cd1 = 0.0f;
                        float* cd2 = reinterpret_cast<float*>(abi + 0x70);
                        if (*cd2 > 0.0f && *cd2 < 10000.0f) *cd2 = 0.0f;
                    }
                    head = *reinterpret_cast<uintptr_t*>(head);
                }
            }
            return true;
        } __except (1) {
            return false;
        }
    }

    void OnTick(uintptr_t game, uintptr_t root) {
        if (!noCd || !root || !game) return;
        uint32_t now = GetTickCount();
        if (now - lastNoCdTick < 50) return;
        lastNoCdTick = now;

        uintptr_t selUnit = GetSelectedUnit(root);
        if (selUnit) {
            ResetUnitCooldowns(selUnit);
        }
    }

    bool HandleCommand(
        const std::wstring& name,
        std::wistringstream& in,
        uintptr_t game,
        uintptr_t root,
        void* (*jassLookup)(const char*),
        std::wstring& statusMsg)
    {
        // 1. Resource Commands
        if (name == L"#gold") {
            int amount = 100000;
            in >> amount;
            if (amount <= 0) amount = 100000;
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            auto fnGetState = reinterpret_cast<int(__cdecl*)(uint32_t, int)>(jassLookup("GetPlayerState"));
            auto fnSetState = reinterpret_cast<void(__cdecl*)(uint32_t, int, int)>(jassLookup("SetPlayerState"));
            if (fnLocal && fnSetState && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal)) && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnSetState))) {
                uint32_t lp = fnLocal();
                int cur = (fnGetState && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnGetState))) ? fnGetState(lp, 1) : 0;
                fnSetState(lp, 1, cur + amount);
                statusMsg = L"Added " + std::to_wstring(amount) + L" gold";
                return true;
            }
            statusMsg = L"Player state unavailable";
            return false;
        }

        if (name == L"#lumber") {
            int amount = 100000;
            in >> amount;
            if (amount <= 0) amount = 100000;
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            auto fnGetState = reinterpret_cast<int(__cdecl*)(uint32_t, int)>(jassLookup("GetPlayerState"));
            auto fnSetState = reinterpret_cast<void(__cdecl*)(uint32_t, int, int)>(jassLookup("SetPlayerState"));
            if (fnLocal && fnSetState && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal)) && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnSetState))) {
                uint32_t lp = fnLocal();
                int cur = (fnGetState && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnGetState))) ? fnGetState(lp, 2) : 0;
                fnSetState(lp, 2, cur + amount);
                statusMsg = L"Added " + std::to_wstring(amount) + L" lumber";
                return true;
            }
            statusMsg = L"Player state unavailable";
            return false;
        }

        if (name == L"#food") {
            int amount = 200;
            in >> amount;
            if (amount <= 0) amount = 200;
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            auto fnSetState = reinterpret_cast<void(__cdecl*)(uint32_t, int, int)>(jassLookup("SetPlayerState"));
            if (fnLocal && fnSetState && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal)) && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnSetState))) {
                uint32_t lp = fnLocal();
                fnSetState(lp, 4, amount);
                statusMsg = L"Food cap set to " + std::to_wstring(amount);
                return true;
            }
            statusMsg = L"Player state unavailable";
            return false;
        }

        if (name == L"#fullres") {
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            auto fnSetState = reinterpret_cast<void(__cdecl*)(uint32_t, int, int)>(jassLookup("SetPlayerState"));
            if (fnLocal && fnSetState && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal)) && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnSetState))) {
                uint32_t lp = fnLocal();
                fnSetState(lp, 1, 1000000);
                fnSetState(lp, 2, 1000000);
                fnSetState(lp, 4, 300);
                statusMsg = L"Full resources applied (1,000,000 Gold, 1,000,000 Lumber, 300 Food)";
                return true;
            }
            statusMsg = L"Player state unavailable";
            return false;
        }

        // 2. Cooldown Commands
        if (name == L"#nocd") {
            noCd = true;
            uintptr_t selUnit = GetSelectedUnit(root);
            if (selUnit) ResetUnitCooldowns(selUnit);
            statusMsg = L"No Cooldown enabled (#nocd)";
            return true;
        }

        if (name == L"#yescd") {
            noCd = false;
            statusMsg = L"No Cooldown disabled (#yescd)";
            return true;
        }

        // 3. Invulnerability Commands
        if (name == L"#invul" || name == L"#vul") {
            bool invul = (name == L"#invul");
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }
            if (detail::TrySetInvulnerable(selUnit, invul)) {
                statusMsg = invul ? L"Selected unit made invulnerable (#invul)" : L"Selected unit vulnerable (#vul)";
                return true;
            }
            statusMsg = L"Error toggling invulnerability";
            return false;
        }

        // 4. Kill Command
        if (name == L"#kill") {
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }
            if (detail::TryKillUnit(selUnit)) {
                statusMsg = L"Selected unit killed (#kill)";
                return true;
            }
            statusMsg = L"Error killing unit";
            return false;
        }

        // 5. Hero Stats Commands
        if (name == L"#lvl") {
            int lvl = 10;
            in >> lvl;
            if (lvl < 1) lvl = 1;
            if (lvl > 100) lvl = 100;
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }
            bool notHero = false;
            if (detail::TrySetHeroLevel(game, jassLookup, selUnit, lvl, notHero)) {
                statusMsg = L"Hero level set to " + std::to_wstring(lvl);
                return true;
            }
            statusMsg = notHero ? L"Selected unit is not a hero" : L"Error setting hero level";
            return false;
        }

        if (name == L"#fullatt" || name == L"#str" || name == L"#agi" || name == L"#int" || name == L"#sp") {
            int val = 500;
            in >> val;
            if (val <= 0) val = 100;
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }
            int statType = 3; // ALL
            if (name == L"#str") statType = 0;
            else if (name == L"#agi") statType = 1;
            else if (name == L"#int") statType = 2;
            else if (name == L"#sp") statType = 4;

            bool notHero = false;
            if (detail::TrySetHeroStats(game, jassLookup, selUnit, statType, val, notHero)) {
                if (statType == 3) statusMsg = L"Hero STR, AGI, INT set to " + std::to_wstring(val);
                else if (statType == 0) statusMsg = L"Hero STR set to " + std::to_wstring(val);
                else if (statType == 1) statusMsg = L"Hero AGI set to " + std::to_wstring(val);
                else if (statType == 2) statusMsg = L"Hero INT set to " + std::to_wstring(val);
                else if (statType == 4) statusMsg = L"Added " + std::to_wstring(val) + L" skill points";
                return true;
            }
            statusMsg = notHero ? L"Selected unit is not a hero" : L"Error modifying hero attributes";
            return false;
        }

        // 6. Teleport Command
        if (name == L"#tele") {
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }
            bool noCoords = false;
            if (detail::TryTeleportUnit(game, jassLookup, selUnit, noCoords)) {
                statusMsg = L"Unit teleported to cursor (#tele)";
                return true;
            }
            statusMsg = noCoords ? L"Could not determine cursor world coordinates" : L"Error teleporting unit";
            return false;
        }

        // 7. Unit & Ability Manipulation Commands (Group 3)
        if (name == L"#create") {
            std::wstring rawcodeStr;
            in >> rawcodeStr;
            if (rawcodeStr.empty()) { statusMsg = L"Usage: #create [rawcode] [count]"; return false; }
            int count = 1;
            in >> count;
            if (count < 1) count = 1;
            if (count > 50) count = 50;

            uint32_t rawcode = detail::ParseRawcode(rawcodeStr);
            if (!rawcode) { statusMsg = L"Invalid rawcode format"; return false; }

            float spawnX = 0.0f, spawnY = 0.0f;
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!detail::TryGetCursorPosition(game, spawnX, spawnY)) {
                if (selUnit) detail::TryGetUnitPosition(selUnit, spawnX, spawnY);
            }

            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            uint32_t lp = (fnLocal && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal))) ? fnLocal() : 0;

            int spawned = 0;
            const float facing = 270.0f;
            for (int i = 0; i < count; ++i) {
                float posX = spawnX + static_cast<float>(i * 40);
                float posY = spawnY;
                uint32_t res = 0;
                if (lp) res = detail::SafeCreateUnit(jassLookup, lp, static_cast<int>(rawcode), posX, posY, facing);
                if (!res) res = detail::SafeCreateItem(jassLookup, static_cast<int>(rawcode), posX, posY);
                if (res) ++spawned;
            }

            statusMsg = L"Spawned " + rawcodeStr + L" (x" + std::to_wstring(spawned > 0 ? spawned : count) + L")";
            return true;
        }

        if (name == L"#copy") {
            int count = 1;
            in >> count;
            if (count < 1) count = 1;
            if (count > 50) count = 50;

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            uint32_t unitTypeId = detail::TryGetUnitTypeId(selUnit);
            if (!unitTypeId) { statusMsg = L"Could not determine unit type ID"; return false; }

            float posX = 0.0f, posY = 0.0f;
            detail::TryGetUnitPosition(selUnit, posX, posY);

            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            uint32_t lp = (fnLocal && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal))) ? fnLocal() : 0;

            int spawned = 0;
            const float facing = 270.0f;
            for (int i = 0; i < count; ++i) {
                float spawnX = posX + static_cast<float>((i + 1) * 50);
                float spawnY = posY;
                uint32_t res = 0;
                if (lp) res = detail::SafeCreateUnit(jassLookup, lp, static_cast<int>(unitTypeId), spawnX, spawnY, facing);
                if (res) ++spawned;
            }

            statusMsg = L"Cloned unit (x" + std::to_wstring(spawned > 0 ? spawned : count) + L")";
            return true;
        }

        if (name == L"#udel") {
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            uint32_t h = detail::TryGetUnitHandle(game, selUnit);
            if (h) detail::SafeRemoveUnit(jassLookup, h);
            detail::TryDeleteUnit(selUnit);

            statusMsg = L"Selected unit deleted (#udel)";
            return true;
        }

        if (name == L"#udmg") {
            float dmg = 100.0f;
            in >> dmg;
            if (dmg <= 0.0f) dmg = 100.0f;

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            if (detail::TryDamageUnit(selUnit, dmg)) {
                statusMsg = L"Dealt " + std::to_wstring(static_cast<int>(dmg)) + L" damage to unit (#udmg)";
                return true;
            }
            statusMsg = L"Error damaging unit";
            return false;
        }

        if (name == L"#udmgb") {
            float dmg = 50.0f;
            in >> dmg;
            if (dmg < 0.0f) dmg = 50.0f;

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            if (detail::TrySetBaseDamage(selUnit, dmg)) {
                statusMsg = L"Set unit base damage to " + std::to_wstring(static_cast<int>(dmg)) + L" (#udmgb)";
                return true;
            }
            statusMsg = L"Error setting base damage";
            return false;
        }

        if (name == L"#sabi") {
            std::wstring rawcodeStr;
            in >> rawcodeStr;
            if (rawcodeStr.empty()) { statusMsg = L"Usage: #sabi [rawcode] [level]"; return false; }
            int level = 1;
            in >> level;
            if (level < 1) level = 1;

            uint32_t rawcode = detail::ParseRawcode(rawcodeStr);
            if (!rawcode) { statusMsg = L"Invalid rawcode format"; return false; }

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            uint32_t h = detail::TryGetUnitHandle(game, selUnit);
            if (h) {
                detail::SafeAddAbility(jassLookup, h, static_cast<int>(rawcode));
                if (level > 1) {
                    detail::SafeSetAbilityLevel(jassLookup, h, static_cast<int>(rawcode), level);
                }
            }

            statusMsg = L"Added ability [" + rawcodeStr + L"] level " + std::to_wstring(level);
            return true;
        }

        if (name == L"#rabi") {
            std::wstring rawcodeStr;
            in >> rawcodeStr;
            if (rawcodeStr.empty()) { statusMsg = L"Usage: #rabi [rawcode]"; return false; }

            uint32_t rawcode = detail::ParseRawcode(rawcodeStr);
            if (!rawcode) { statusMsg = L"Invalid rawcode format"; return false; }

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            uint32_t h = detail::TryGetUnitHandle(game, selUnit);
            if (h) {
                detail::SafeRemoveAbility(jassLookup, h, static_cast<int>(rawcode));
            }

            statusMsg = L"Removed ability [" + rawcodeStr + L"]";
            return true;
        }

        if (name == L"#addall" || name == L"#raddall") {
            bool isAdd = (name == L"#addall");
            std::wstring rawcodeStr;
            in >> rawcodeStr;
            if (rawcodeStr.empty()) {
                statusMsg = isAdd ? L"Usage: #addall [rawcode]" : L"Usage: #raddall [rawcode]";
                return false;
            }

            uint32_t rawcode = detail::ParseRawcode(rawcodeStr);
            if (!rawcode) { statusMsg = L"Invalid rawcode format"; return false; }

            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            auto fnCreateGroup = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("CreateGroup"));
            auto fnEnumPlayer = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t, uint32_t)>(jassLookup("GroupEnumUnitsOfPlayer"));
            auto fnFirstOfGroup = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(jassLookup("FirstOfGroup"));
            auto fnGroupRemove = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(jassLookup("GroupRemoveUnit"));
            auto fnDestroyGroup = reinterpret_cast<void(__cdecl*)(uint32_t)>(jassLookup("DestroyGroup"));

            uint32_t lp = (fnLocal && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal))) ? fnLocal() : 0;
            if (fnCreateGroup && fnEnumPlayer && fnFirstOfGroup && fnGroupRemove && fnDestroyGroup && lp &&
                detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnCreateGroup)) &&
                detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnEnumPlayer)) &&
                detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnFirstOfGroup)) &&
                detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnGroupRemove)) &&
                detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnDestroyGroup))) {
                uint32_t grp = fnCreateGroup();
                if (grp) {
                    fnEnumPlayer(grp, lp, 0);
                    uint32_t u = 0;
                    while ((u = fnFirstOfGroup(grp)) != 0) {
                        if (isAdd) detail::SafeAddAbility(jassLookup, u, static_cast<int>(rawcode));
                        else detail::SafeRemoveAbility(jassLookup, u, static_cast<int>(rawcode));
                        fnGroupRemove(grp, u);
                    }
                    fnDestroyGroup(grp);
                }
            } else {
                uintptr_t selUnit = GetSelectedUnit(root);
                if (selUnit) {
                    uint32_t h = detail::TryGetUnitHandle(game, selUnit);
                    if (h) {
                        if (isAdd) detail::SafeAddAbility(jassLookup, h, static_cast<int>(rawcode));
                        else detail::SafeRemoveAbility(jassLookup, h, static_cast<int>(rawcode));
                    }
                }
            }

            statusMsg = isAdd ? (L"Added ability [" + rawcodeStr + L"] to player units")
                              : (L"Removed ability [" + rawcodeStr + L"] from player units");
            return true;
        }

        // 8. Unit Inspection, Positioning & Ability Button Control Commands (Group 4)
        if (name == L"#size") {
            float val = 100.0f;
            in >> val;
            float scale = (val > 10.0f) ? (val / 100.0f) : val;
            if (scale < 0.05f) scale = 0.05f;
            if (scale > 20.0f) scale = 20.0f;

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            uint32_t h = detail::TryGetUnitHandle(game, selUnit);
            if (h) detail::SafeSetUnitScale(jassLookup, h, scale, scale, scale);

            wchar_t sbuf[64];
            swprintf_s(sbuf, L"Unit scale set to %.2fx (#size)", scale);
            statusMsg = sbuf;
            return true;
        }

        if (name == L"#upos") {
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            float ux = 0.0f, uy = 0.0f;
            if (detail::TryGetUnitPosition(selUnit, ux, uy)) {
                wchar_t buf[64];
                swprintf_s(buf, L"X: %.5f, Y: %.5f", ux, uy);
                statusMsg = buf;
                return true;
            }
            statusMsg = L"Could not determine unit position";
            return false;
        }

        if (name == L"#mpos") {
            float mx = 0.0f, my = 0.0f;
            detail::TryGetCursorPosition(game, mx, my);
            int resW = GetSystemMetrics(SM_CXSCREEN);
            int resH = GetSystemMetrics(SM_CYSCREEN);
            wchar_t buf[128];
            swprintf_s(buf, L"X: %.5f, Y: %.5f, Z: 0.00000 | Res: %d, %d", mx, my, resW, resH);
            statusMsg = buf;

            uintptr_t selUnit = GetSelectedUnit(root);
            if (selUnit) {
                float ux = 0.0f, uy = 0.0f;
                if (detail::TryGetUnitPosition(selUnit, ux, uy)) {
                    wchar_t ubuf[64];
                    swprintf_s(ubuf, L" | UnitXY: %.5f, %.5f", ux, uy);
                    statusMsg += ubuf;
                }
            }
            return true;
        }

        if (name == L"#ppos") {
            std::wstring arg1;
            in >> arg1;
            if (!arg1.empty()) {
                float px = static_cast<float>(_wtof(arg1.c_str()));
                float py = 0.0f;
                in >> py;
                detail::SafePanCameraTo(jassLookup, px, py);
                wchar_t buf[64];
                swprintf_s(buf, L"Camera panned to (%.2f, %.2f)", px, py);
                statusMsg = buf;
            } else {
                float cx = 0.0f, cy = 0.0f;
                detail::TryGetCursorPosition(game, cx, cy);
                wchar_t buf[64];
                swprintf_s(buf, L"Camera/CursorXY: [%.2f, %.2f]", cx, cy);
                statusMsg = buf;
            }
            return true;
        }

        if (name == L"#cname") {
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            uint32_t typeId = detail::TryGetUnitTypeId(selUnit);
            std::wstring codeStr = detail::RawcodeToWString(typeId);
            statusMsg = L"|cFFff9900Unit Name|r: [" + codeStr + L"] |cFFff9900ID|r: " + codeStr;
            return true;
        }

        if (name == L"#islot") {
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            uint32_t h = detail::TryGetUnitHandle(game, selUnit);
            std::wstring slotsInfo = L"Item Slots: ";
            int foundCount = 0;
            for (int s = 0; s < 6; ++s) {
                uint32_t itm = detail::SafeUnitItemInSlot(jassLookup, h, s);
                if (itm) {
                    uint32_t itmType = detail::SafeGetItemTypeId(jassLookup, itm);
                    std::wstring itmCode = detail::RawcodeToWString(itmType);
                    slotsInfo += L"[" + std::to_wstring(s) + L": " + itmCode + L"] ";
                    ++foundCount;
                }
            }
            if (foundCount == 0) slotsInfo += L"[Empty]";
            statusMsg = slotsInfo;
            return true;
        }

        if (name == L"#uflag") {
            POINT pt{ 0, 0 };
            GetCursorPos(&pt);
            HWND hwnd = FindWindowA("Warcraft III", nullptr);
            if (!hwnd) hwnd = GetActiveWindow();
            POINT clientPt = pt;
            if (hwnd) ScreenToClient(hwnd, &clientPt);

            wchar_t buf[128];
            swprintf_s(buf, L"MouseXY: [%d, %d], ClientXY: [%d, %d]", pt.x, pt.y, clientPt.x, clientPt.y);
            statusMsg = buf;

            uintptr_t selUnit = GetSelectedUnit(root);
            if (selUnit) {
                uint32_t flags = detail::TryGetUnitFlags(selUnit);
                wchar_t fbuf[64];
                swprintf_s(fbuf, L" | Unit Flags: 0x%08X", flags);
                statusMsg += fbuf;
            }
            return true;
        }

        if (name == L"#wsize") {
            int w = 0, h = 0;
            in >> w >> h;
            if (w <= 0 || h <= 0) { statusMsg = L"Usage: #wsize [width] [height]"; return false; }
            HWND hwnd = FindWindowA("Warcraft III", nullptr);
            if (!hwnd) hwnd = GetActiveWindow();
            if (hwnd) {
                SetWindowPos(hwnd, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
                statusMsg = L"Window size set to " + std::to_wstring(w) + L"x" + std::to_wstring(h);
                return true;
            }
            statusMsg = L"Could not find game window";
            return false;
        }

        if (name == L"#hideb" || name == L"#showb") {
            bool isHide = (name == L"#hideb");
            std::wstring rawcodeStr;
            in >> rawcodeStr;
            if (rawcodeStr.empty()) {
                statusMsg = isHide ? L"Usage: #hideb [rawcode]" : L"Usage: #showb [rawcode]";
                return false;
            }
            uint32_t rawcode = detail::ParseRawcode(rawcodeStr);
            if (!rawcode) { statusMsg = L"Invalid rawcode format"; return false; }

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) { statusMsg = L"No unit selected"; return false; }

            detail::TryModifyAbilityButton(selUnit, rawcode, isHide ? 1 : -1);
            statusMsg = (isHide ? L"Hide Ability: " : L"Show Ability: ") + rawcodeStr;
            return true;
        }

        if (name == L"#hideball" || name == L"#showball") {
            bool isHide = (name == L"#hideball");
            std::wstring rawcodeStr;
            in >> rawcodeStr;
            if (rawcodeStr.empty()) {
                statusMsg = isHide ? L"Usage: #hideball [rawcode]" : L"Usage: #showball [rawcode]";
                return false;
            }
            uint32_t rawcode = detail::ParseRawcode(rawcodeStr);
            if (!rawcode) { statusMsg = L"Invalid rawcode format"; return false; }

            uintptr_t selUnit = GetSelectedUnit(root);
            if (selUnit) {
                detail::TryModifyAbilityButton(selUnit, rawcode, isHide ? 1 : -1);
            }
            statusMsg = (isHide ? L"Hide Ability All: " : L"Show Ability All: ") + rawcodeStr;
            return true;
        }

        if (name == L"#ufind" || name == L"#ufinds") {
            std::wstring term;
            in >> term;
            if (term.empty()) {
                statusMsg = (name == L"#ufind") ? L"Usage: #ufind [rawcode]" : L"Usage: #ufinds [name]";
                return false;
            }
            uintptr_t selUnit = GetSelectedUnit(root);
            if (selUnit) {
                float ux = 0.0f, uy = 0.0f;
                detail::TryGetUnitPosition(selUnit, ux, uy);
                wchar_t fbuf[128];
                swprintf_s(fbuf, L"Found [%s] at (%.1f, %.1f)", term.c_str(), ux, uy);
                statusMsg = fbuf;
                return true;
            }
            statusMsg = L"Unit [" + term + L"] not found in active selection";
            return false;
        }

        if (name == L"#selc") {
            std::wstring rawcodeStr;
            in >> rawcodeStr;
            uintptr_t selUnit = GetSelectedUnit(root);
            if (selUnit) {
                uint32_t h = detail::TryGetUnitHandle(game, selUnit);
                if (h) {
                    detail::SafeClearSelection(jassLookup);
                    detail::SafeSelectUnit(jassLookup, h, true);
                }
            }
            statusMsg = rawcodeStr.empty() ? L"Selected unit" : (L"Selected unit [" + rawcodeStr + L"]");
            return true;
        }

        // 9. Match Utility, Speed & View Commands (Group 5)
        if (name == L"#speed") {
            float spd = 1.0f;
            in >> spd;
            if (spd < 1.0f || spd > 70.0f) {
                statusMsg = L"|CFFFCD211Limited to only 1.0 to 70.0|r";
                return false;
            }
            gameSpeed = spd;
            int jassSpd = (spd <= 1.0f) ? 2 : ((spd <= 2.0f) ? 3 : 4);
            detail::SafeSetGameSpeed(jassLookup, jassSpd);

            wchar_t sbuf[64];
            swprintf_s(sbuf, L"|CFFFCD211Set Speed|r: %.1f", spd);
            statusMsg = sbuf;
            return true;
        }

        if (name == L"#wide") {
            widescreen = !widescreen;
            statusMsg = widescreen ? L"|cff44ff3dEnable Wide Screen|r" : L"|cffff5c5cDisable Wide Screen|r";
            return true;
        }

        if (name == L"#lock") {
            cursorLocked = !cursorLocked;
            HWND hwnd = FindWindowA("Warcraft III", nullptr);
            if (!hwnd) hwnd = GetActiveWindow();
            if (cursorLocked && hwnd) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                POINT tl = { rc.left, rc.top };
                POINT br = { rc.right, rc.bottom };
                ClientToScreen(hwnd, &tl);
                ClientToScreen(hwnd, &br);
                RECT rcScreen = { tl.x, tl.y, br.x, br.y };
                ClipCursor(&rcScreen);
            } else {
                ClipCursor(nullptr);
            }
            statusMsg = cursorLocked ? L"|cffff5c5cLock Cursor|r" : L"|cff44ff3dUnlock Cursor|r";
            return true;
        }

        if (name == L"#menu") {
            menuVisible = !menuVisible;
            statusMsg = menuVisible ? L"|cff44ff3dMenu Opened|r (#menu)" : L"|cffff5c5cMenu Closed|r (#menu)";
            return true;
        }

        if (name == L"#ping") {
            statusMsg = network_ping::Status();
            return true;
        }

        if (name == L"#mute" || name == L"#unmute") {
            bool isMute = (name == L"#mute");
            int slot = -1;
            std::wstring extra;
            if (!(in >> slot) || slot < 1 || slot > 16 || (in >> extra)) {
                statusMsg = isMute ? L"Usage: #mute [slot 1-16]" : L"Usage: #unmute [slot 1-16]";
                return false;
            }
            --slot; // User-facing slots are 1-based; native sender IDs are 0-based.

            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            uint32_t lp = (fnLocal && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal))) ? fnLocal() : 0;
            uint32_t targetPlayer = detail::SafePlayer(jassLookup, slot);
            if (!lp || !targetPlayer) {
                statusMsg = L"Player unavailable";
                return false;
            }
            if (targetPlayer && targetPlayer == lp) {
                statusMsg = isMute ? L"|CFFFCD211Can't mute myself|r" : L"|CFFFCD211Can't Remove mute myself|r";
                return false;
            }

            auto it = std::find(mutedSlots.begin(), mutedSlots.end(), slot);
            if (isMute) {
                if (it == mutedSlots.end()) mutedSlots.push_back(slot);
                mutedMask.fetch_or(1u << slot, std::memory_order_relaxed);
                statusMsg = L"|CFFFCD211Mute Player|r: " + std::to_wstring(slot + 1);
            } else {
                if (it != mutedSlots.end()) mutedSlots.erase(it);
                mutedMask.fetch_and(~(1u << slot), std::memory_order_relaxed);
                statusMsg = L"|CFFFCD211Remove Mute Player|r: " + std::to_wstring(slot + 1);
            }
            return true;
        }

        if (name == L"#ggl" || name == L"#gll") {
            int slot = -1;
            int gold = 0, lumber = 0;
            if (name == L"#ggl") {
                in >> slot >> gold >> lumber;
            } else {
                in >> slot >> lumber >> gold;
            }

            if (in.fail() || slot < 1 || slot > 24 || gold < 0 || lumber < 0 || (gold == 0 && lumber == 0)) {
                statusMsg = (name == L"#ggl") ? L"Usage: #ggl [player 1-24] [gold] [lumber]"
                                              : L"Usage: #gll [player 1-24] [lumber] [gold]";
                return false;
            }

            auto fnGetState = reinterpret_cast<int(__cdecl*)(uint32_t, int)>(jassLookup("GetPlayerState"));
            auto fnSetState = reinterpret_cast<void(__cdecl*)(uint32_t, int, int)>(jassLookup("SetPlayerState"));
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            uint32_t lp = (fnLocal && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal))) ? fnLocal() : 0;
            uint32_t targetPlayer = detail::SafePlayer(jassLookup, slot - 1);

            if (fnGetState && fnSetState && targetPlayer &&
                detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnGetState)) &&
                detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnSetState))) {
                if (lp && lp != targetPlayer) {
                    int curGold = fnGetState(lp, 1);
                    int curLumber = fnGetState(lp, 2);
                    if (gold > 0) fnSetState(lp, 1, std::max(0, curGold - gold));
                    if (lumber > 0) fnSetState(lp, 2, std::max(0, curLumber - lumber));
                }
                int tgtGold = fnGetState(targetPlayer, 1);
                int tgtLumber = fnGetState(targetPlayer, 2);
                if (gold > 0) fnSetState(targetPlayer, 1, tgtGold + gold);
                if (lumber > 0) fnSetState(targetPlayer, 2, tgtLumber + lumber);

                wchar_t buf[128];
                swprintf_s(buf, L"Transferred %d Gold and %d Lumber to Player %d", gold, lumber, slot);
                statusMsg = buf;
                return true;
            }
            statusMsg = L"Error transferring resources";
            return false;
        }

        if (name == L"#pcam" || name == L"#pzoom") {
            float dist = 2400.0f;
            in >> dist;
            if (dist < 400.0f) dist = 400.0f;
            if (dist > 6000.0f) dist = 6000.0f;
            presetCamDist = dist;

            detail::SafeSetCameraField(jassLookup, 0, dist, 0.0f);
            wchar_t buf[64];
            swprintf_s(buf, L"Camera preset distance set to %.0f", dist);
            statusMsg = buf;
            return true;
        }

        if (name == L"#nhailai") {
            std::wstring arg,extra;
            if(!(in>>arg)){repeatSlot=-1;repeatAll=false;pendingChatMessages.clear();statusMsg=L"Repeat chat disabled";return true;}
            if(in>>extra){statusMsg=L"Usage: #nhailai <1..16|all>; #nhailai to stop";return false;}
            if(arg==L"all"){repeatSlot=-1;repeatAll=!repeatAll;pendingChatMessages.clear();statusMsg=repeatAll?L"Repeat all chat enabled":L"Repeat chat disabled";return true;}
            uint32_t slot=0;
            if(!TransferNumber(arg,16,slot)||!slot){statusMsg=L"Slot must be 1..16";return false;}
            repeatSlot=static_cast<int>(slot)-1;repeatAll=false;
            pendingChatMessages.clear();
            statusMsg=L"Repeat chat enabled for Player "+std::to_wstring(slot);
            return true;
        }

        if (name == L"#mtrade") {
            std::wstring extra;if(in>>extra){statusMsg=L"Usage: #mtrade";return false;}
            auto fn=reinterpret_cast<void(__cdecl*)(int,int)>(jassLookup("SetMapFlag"));
            if(!fn||!detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fn))){statusMsg=L"Trade map flag unavailable";return false;}
            fn(0x100,0);
            statusMsg=L"Trade restriction disabled";
            return true;
        }

        if (name == L"#gmodel") {
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) {
                statusMsg = L"No unit selected";
                return false;
            }
            char modelBuf[260]{};
            if (detail::TryGetUnitModel(selUnit, modelBuf, sizeof(modelBuf))) {
                wchar_t wbuf[260];
                size_t converted = 0;
                mbstowcs_s(&converted, wbuf, modelBuf, _TRUNCATE);
                statusMsg = L"|CFFFCD211Dir Model|r: " + std::wstring(wbuf);
                return true;
            }
            statusMsg = L"|CFFFCD211Dir Model|r: default";
            return true;
        }

        if (name == L"#emodel") {
            std::wstring path;
            in >> path;
            customModelPath = path;
            statusMsg = L"|CFFFCD211Edit Model|r: " + path;
            return true;
        }

        if (name == L"#smodel") {
            std::wstring path;
            in >> path;
            customModelPath = path;
            statusMsg = L"|CFFFCD211Set Model|r: " + path;
            return true;
        }

        if (name == L"#repeat") {
            std::wstring arg,text;uint32_t count=0;
            if(!(in>>arg)){statusMsg=L"Usage: #repeat <1..10000> <text>; #repeat stop";return false;}
            if(arg==L"stop"){
                if(in>>text){statusMsg=L"Usage: #repeat stop";return false;}
                repeatText.clear();repeatRemaining=0;statusMsg=L"Repeat stopped";return true;
            }
            if(!TransferNumber(arg,10000,count)||!count){statusMsg=L"Repeat count must be 1..10000";return false;}
            std::getline(in>>std::ws,text);
            const auto encoded=detail::Utf8Narrow(text);
            if(encoded.empty()||encoded.size()>128||encoded.front()=='#'){
                statusMsg=L"Repeat text must be 1..128 UTF-8 bytes and cannot start with #";return false;
            }
            repeatText=encoded;repeatRemaining=count;nextRepeat=0;
            statusMsg=L"Repeat queued: "+std::to_wstring(count)+L" messages";return true;
        }

        if (name == L"#spam") {
            std::wstring arg,extra;uint32_t slot=0;
            if(!(in>>arg)){spamSlot=-1;statusMsg=L"Alliance spam disabled";return true;}
            if((in>>extra)||!TransferNumber(arg,16,slot)||!slot){statusMsg=L"Usage: #spam <1..16>; #spam to stop";return false;}
            spamSlot=static_cast<int>(slot)-1;nextSpam=0;
            statusMsg=L"Alliance spam enabled for Player "+std::to_wstring(slot);
            return true;
        }

        if (name == L"#shareall") {
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            uint32_t lp = (fnLocal && detail::SafeIsExecutable(reinterpret_cast<uintptr_t>(fnLocal))) ? fnLocal() : 0;
            if (lp) {
                for (int i = 0; i < 16; ++i) {
                    uint32_t p = detail::SafePlayer(jassLookup, i);
                    if (p && p != lp) {
                        detail::SafeSetPlayerAlliance(jassLookup, lp, p, 6, true);
                    }
                }
            }
            statusMsg = L"|cff44ff3dShared control with all allies";
            return true;
        }

        if (name == L"#spamall") {
            std::wstring extra;if(in>>extra){statusMsg=L"Usage: #spamall";return false;}
            spamAll=!spamAll;nextSpam=0;
            statusMsg=spamAll?L"Alliance spam to all enabled":L"Alliance spam to all disabled";
            return true;
        }

        if (name == L"#save") {
            std::string saveName = "save.w3z";
            std::wstring wName;
            if (in >> wName && !wName.empty()) {
                char buf[64]{};
                size_t c = 0;
                wcstombs_s(&c, buf, wName.c_str(), _TRUNCATE);
                saveName = buf;
            }
            detail::SafeSaveGame(jassLookup, saveName.c_str());
            statusMsg = L"Game saved";
            return true;
        }

        if (name == L"#srand") {
            unsigned int seed = 0;
            in >> seed;
            srand(seed);
            statusMsg = L"Random seed set to " + std::to_wstring(seed);
            return true;
        }

        if (name == L"#findCP") {
            statusMsg = L"|cFFff9900JJCP: |rNone detected";
            return true;
        }

        if (name == L"#quickdrop") {
            int val = -1;
            if (in >> val) {
                quickDrop = (val != 0);
            } else {
                quickDrop = !quickDrop;
            }
            statusMsg = quickDrop ? L"|cff44ff3dQuick Drop Item (Shift+RightClick): ON" : L"|cffff4444Quick Drop Item (Shift+RightClick): OFF";
            return true;
        }

        if (name == L"#dropall" || name == L"#drop") {
            ItemManager im;
            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) {
                selUnit = im.ResolveHeroUnit(root, game);
            }
            if (!selUnit) {
                statusMsg = L"|cffff4444No unit selected to drop items";
                return true;
            }
            int count = im.DropAllItems(game, selUnit);
            wchar_t buf[64];
            swprintf_s(buf, L"|cff44ff3dDropped %d items", count);
            statusMsg = buf;
            return true;
        }

        if (name == L"#autol" || name == L"#autoclickl") {
            int val = -1;
            if (in >> val) {
                autoClickLeft = (val != 0);
            } else {
                autoClickLeft = !autoClickLeft;
            }
            statusMsg = autoClickLeft ? L"Set Auto Click Left: |cff44ff3dOn" : L"Set Auto Click Left: |cffff5c5cOff";
            return true;
        }

        if (name == L"#autor" || name == L"#autoclickr") {
            int val = -1;
            if (in >> val) {
                autoClickRight = (val != 0);
            } else {
                autoClickRight = !autoClickRight;
            }
            statusMsg = autoClickRight ? L"Set Auto Click Right: |cff44ff3dOn" : L"Set Auto Click Right: |cffff5c5cOff";
            return true;
        }

        if (name == L"#clickdelay") {
            int val = 10;
            if (in >> val && val >= 5 && val <= 1000) {
                autoClickInterval = static_cast<unsigned>(val);
                wchar_t buf[64];
                swprintf_s(buf, L"|cff44ff3dAuto Click Interval set to %d ms", val);
                statusMsg = buf;
            } else {
                statusMsg = L"|cffff4444Usage: #clickdelay <5-1000>";
            }
            return true;
        }

        if (name == L"#ms") {
            float spd = 522.0f;
            if (!(in >> spd)) spd = 522.0f;
            if (spd < 0.0f) spd = 0.0f;
            if (spd > 2000.0f) spd = 2000.0f;

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) {
                statusMsg = L"|cffff4444No unit selected";
                return true;
            }

            uint32_t h = detail::TryGetUnitHandle(game, selUnit);
            if (h) {
                detail::SafeSetUnitMoveSpeed(jassLookup, h, spd);
            }
            wchar_t buf[64];
            swprintf_s(buf, L"|cff44ff3dUnit move speed set to %.0f", spd);
            statusMsg = buf;
            return true;
        }

        if (name == L"#as") {
            float factor = 2.0f;
            if (!(in >> factor)) factor = 2.0f;
            if (factor < 0.1f) factor = 0.1f;
            if (factor > 50.0f) factor = 50.0f;

            uintptr_t selUnit = GetSelectedUnit(root);
            if (!selUnit) {
                statusMsg = L"|cffff4444No unit selected";
                return true;
            }

            bool ok = detail::TrySetUnitAttackSpeed(selUnit, factor);
            if (ok) {
                wchar_t buf[64];
                swprintf_s(buf, L"|cff44ff3dUnit attack speed multiplier set to %.2fx", factor);
                statusMsg = buf;
            } else {
                statusMsg = L"|cffff4444Failed to set unit attack speed";
            }
            return true;
        }

        if (name == L"#mh") {
            int arg = -1;
            bool enable = false;
            if (in >> arg) {
                enable = (arg != 0);
            } else {
                enable = (activeFogModifier == 0);
            }

            if (enable) {
                if (activeFogModifier != 0) {
                    statusMsg = L"|cff44ff3dMaphack already ON";
                    return true;
                }
                auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
                uint32_t lp = fnLocal ? fnLocal() : 0;
                uint32_t bounds = detail::SafeGetWorldBounds(jassLookup);
                if (!lp || !bounds) {
                    statusMsg = L"|cffff4444Maphack failed: cannot obtain player or bounds";
                    return true;
                }
                uint32_t fogMod = detail::SafeCreateFogModifier(jassLookup, lp, 4 /* FOG_OF_WAR_VISIBLE */, bounds, true, false);
                if (fogMod) {
                    detail::SafeFogModifierStart(jassLookup, fogMod);
                    activeFogModifier = fogMod;
                    statusMsg = L"|cff44ff3dMaphack: ON";
                } else {
                    statusMsg = L"|cffff4444Maphack failed: CreateFogModifierRect returned null";
                }
            } else {
                if (activeFogModifier == 0) {
                    statusMsg = L"|cffff4444Maphack already OFF";
                    return true;
                }
                detail::SafeFogModifierStop(jassLookup, activeFogModifier);
                detail::SafeDestroyFogModifier(jassLookup, activeFogModifier);
                activeFogModifier = 0;
                statusMsg = L"|cffff4444Maphack: OFF";
            }
            return true;
        }

        // 17. Courier & MTH Commands
        if (name == L"#ga" || name == L"#courier" || name == L"#chuyendo") {
            std::wstring sub;
            in >> sub;
            if (sub == L"1" || sub == L"2" || sub == L"3") {
                int idx = sub[0] - L'1';
                if (courierMgr) return courierMgr->ExecuteMthRoute(0, idx, game, root, jassLookup, statusMsg);
                statusMsg = L"Courier manager not ready";
                return false;
            } else if (sub == L"back") {
                if (courierMgr) return courierMgr->OrderReturn(game, jassLookup, statusMsg);
                statusMsg = L"Courier manager not ready";
                return false;
            } else if (sub == L"set") {
                uintptr_t sel = GetSelectedUnit(root);
                if (courierMgr && sel) {
                    uint32_t h = detail::TryGetUnitHandle(game, sel);
                    if (h) {
                        courierMgr->activeCourierHandle = h;
                        statusMsg = L"Da gan unit lam Ga thanh cong!";
                        return true;
                    }
                }
                statusMsg = L"Khong co unit duoc chon de lam Ga";
                return false;
            } else if (sub == L"stop") {
                if (courierMgr) {
                    courierMgr->StopMth();
                    statusMsg = L"Da dung chay MTH / Ga!";
                    return true;
                }
                statusMsg = L"Courier manager not ready";
                return false;
            } else if (sub == L"base") {
                uintptr_t sel = GetSelectedUnit(root);
                float x = 0.0f, y = 0.0f;
                if (sel && detail::TryGetUnitPosition(sel, x, y)) {
                    if (courierMgr) {
                        courierMgr->courierBaseX = x;
                        courierMgr->courierBaseY = y;
                        courierMgr->hasBasePos = true;
                        statusMsg = L"Da dat can cu Ga tai: (" + std::to_wstring((int)x) + L", " + std::to_wstring((int)y) + L")";
                        return true;
                    }
                }
                statusMsg = L"Khong lay duoc toa do can cu";
                return false;
            } else {
                if (courierMgr) return courierMgr->OrderDeliver(game, root, jassLookup, statusMsg);
                statusMsg = L"Courier manager not ready";
                return false;
            }
        }

        if (name == L"#chayga") {
            int idx = 1;
            in >> idx;
            if (idx < 1 || idx > 3) idx = 1;
            if (courierMgr) return courierMgr->ExecuteMthRoute(0, idx - 1, game, root, jassLookup, statusMsg);
            statusMsg = L"Courier manager not ready";
            return false;
        }

        if (name == L"#chaysoi" || name == L"#soi" || name == L"#caysoi") {
            int idx = 1;
            in >> idx;
            if (idx < 1 || idx > 3) idx = 1;
            if (courierMgr) return courierMgr->ExecuteMthRoute(1, idx - 1, game, root, jassLookup, statusMsg);
            statusMsg = L"Courier manager not ready";
            return false;
        }

        if (name == L"#setga") {
            uintptr_t sel = GetSelectedUnit(root);
            if (courierMgr && sel) {
                uint32_t h = detail::TryGetUnitHandle(game, sel);
                if (h) {
                    courierMgr->activeCourierHandle = h;
                    statusMsg = L"Da gan unit lam Ga thanh cong!";
                    return true;
                }
            }
            statusMsg = L"Khong co unit duoc chon de lam Ga";
            return false;
        }

        if (name == L"#backga" || name == L"#backsoi") {
            if (courierMgr) return courierMgr->OrderReturn(game, jassLookup, statusMsg);
            statusMsg = L"Courier manager not ready";
            return false;
        }

        if (name == L"#gabase") {
            uintptr_t sel = GetSelectedUnit(root);
            float x = 0.0f, y = 0.0f;
            if (sel && detail::TryGetUnitPosition(sel, x, y)) {
                if (courierMgr) {
                    courierMgr->courierBaseX = x;
                    courierMgr->courierBaseY = y;
                    courierMgr->hasBasePos = true;
                    statusMsg = L"Da dat can cu Ga tai: (" + std::to_wstring((int)x) + L", " + std::to_wstring((int)y) + L")";
                    return true;
                }
            }
            statusMsg = L"Khong lay duoc toa do can cu";
            return false;
        }

        if (name == L"#stopga" || name == L"#stopmth") {
            if (courierMgr) {
                courierMgr->StopMth();
                statusMsg = L"Da dung chay MTH / Ga!";
                return true;
            }
            statusMsg = L"Courier manager not ready";
            return false;
        }

        return false;
    }
};

} // namespace wht
