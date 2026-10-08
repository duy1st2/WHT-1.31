#pragma once
#include <windows.h>
#include <cstdint>
#include <cmath>
#include <vector>
#include <string>
#include "auto_pick.hpp"
#include "sell_all.hpp"

namespace wht {

class ItemManager {
public:
    bool quickDropEnabled = false;
    bool autoPickEnabled = false;
    bool autoPickGokuEnabled = false;
    bool autoPickGokuCompleted = false;
    AutoPick autoPick;

    inline bool SafeIsExecutable(uintptr_t at) {
        MEMORY_BASIC_INFORMATION m{};
        return at && VirtualQuery(reinterpret_cast<void*>(at), &m, sizeof(m)) == sizeof(m) &&
            m.State == MEM_COMMIT && !(m.Protect & (PAGE_GUARD | PAGE_NOACCESS)) &&
            (m.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
    }

    inline bool SafeIsReadable(const void* p, size_t n) {
        MEMORY_BASIC_INFORMATION m{};
        if (!p || !n || VirtualQuery(p, &m, sizeof(m)) != sizeof(m) || m.State != MEM_COMMIT ||
            (m.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
        uintptr_t start = reinterpret_cast<uintptr_t>(p);
        uintptr_t end = reinterpret_cast<uintptr_t>(m.BaseAddress) + m.RegionSize;
        return start <= end && n <= end - start;
    }

    uint32_t GetUnitHandle(uintptr_t game, uintptr_t unit) {
        if (!unit || !game || unit < 0x10000 || unit >= 0x7FFF0000) return 0;
        if (!SafeIsReadable(reinterpret_cast<void*>(unit), 0x20)) return 0;
        __try {
            uintptr_t umRva = ActiveGameOffsets().getUnitManager;
            uintptr_t luRva = ActiveGameOffsets().lookupUnit;
            uintptr_t wRva = ActiveGameOffsets().world;
            if (SafeIsExecutable(game + umRva) && SafeIsExecutable(game + luRva) &&
                SafeIsReadable(reinterpret_cast<void*>(game + wRva), 4)) {
                uintptr_t state = *reinterpret_cast<uintptr_t*>(game + wRva);
                if (state && SafeIsReadable(reinterpret_cast<void*>(state), 0x20)) {
                    auto getContext = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(game + umRva);
                    uintptr_t ctx = getContext(state);
                    if (ctx && SafeIsReadable(reinterpret_cast<void*>(ctx), 0x20)) {
                        auto getHandle = reinterpret_cast<uint32_t(__thiscall*)(uintptr_t, uintptr_t, int)>(game + luRva);
                        uint32_t h = getHandle(ctx, unit, 0); // Exact ETS.mix 10074330 arg 0
                        if (h) return h;
                        h = getHandle(ctx, unit, 1);
                        if (h) return h;
                    }
                }
            }
            return 0;
        } __except (1) {
            return 0;
        }
    }

    bool quickDropActive = false;
    bool dropAllActive = false;
    uint32_t dropAllHeroHandle = 0;
    float dropAllHeroX = 0.0f;
    float dropAllHeroY = 0.0f;
    int dropAllSlot = 0;
    int dropAllPasses = 0;
    uint32_t lastDropTick = 0;
    uint32_t lastDropAllCompleteTick = 0;

    void StartDropAll(uint32_t heroHandle, float heroX, float heroY) {
        dropAllActive = true;
        dropAllHeroHandle = heroHandle;
        dropAllHeroX = heroX;
        dropAllHeroY = heroY;
        dropAllSlot = 0;
        dropAllPasses = 0;
        lastDropTick = 0;
    }

    bool sellAllActive = false;
    uint32_t lastSellAllCompleteTick = 0;
    SellAll sellAll;
    std::string sellFeedback;

    bool DropItemFromSlot(uintptr_t game, uintptr_t unit, int slot) {
        if (!game || !unit || slot < 0 || slot >= 6) return false;
        __try {
            uint32_t unitHandle = GetUnitHandle(game, unit);
            if (!unitHandle) return false;

            auto pUnitItemInSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(LookupJassNative(game, "UnitItemInSlot"));
            auto pUnitRemoveItemFromSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(LookupJassNative(game, "UnitRemoveItemFromSlot"));
            auto pUnitRemoveItem = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(LookupJassNative(game, "UnitRemoveItem"));

            uint32_t itemHandle = pUnitItemInSlot ? pUnitItemInSlot(unitHandle, slot) : 0;
            if (pUnitRemoveItemFromSlot) {
                pUnitRemoveItemFromSlot(unitHandle, slot);
            }
            if (itemHandle && pUnitRemoveItem && pUnitItemInSlot && pUnitItemInSlot(unitHandle, slot) != 0) {
                pUnitRemoveItem(unitHandle, itemHandle);
            }
            lastDropAllCompleteTick = GetTickCount();
            return true;
        } __except (1) {
            return false;
        }
    }

    int DropAllItems(uintptr_t game, uintptr_t unit) {
        if (!game || !unit) return 0;
        __try {
            uint32_t unitHandle = GetUnitHandle(game, unit);
            if (!unitHandle) return 0;

            auto pUnitItemInSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(LookupJassNative(game, "UnitItemInSlot"));
            auto pUnitRemoveItemFromSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(LookupJassNative(game, "UnitRemoveItemFromSlot"));
            auto pUnitRemoveItem = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(LookupJassNative(game, "UnitRemoveItem"));

            int dropped = 0;
            for (int pass = 0; pass < 6; ++pass) {
                bool foundAny = false;
                for (int s = 0; s < 6; ++s) {
                    __try {
                        uint32_t itemHandle = pUnitItemInSlot ? pUnitItemInSlot(unitHandle, s) : 0;
                        if (itemHandle || !pUnitItemInSlot) {
                            if (pUnitRemoveItemFromSlot) {
                                pUnitRemoveItemFromSlot(unitHandle, s);
                                dropped++;
                                foundAny = true;
                            }
                            if (itemHandle && pUnitRemoveItem && pUnitItemInSlot && pUnitItemInSlot(unitHandle, s) != 0) {
                                pUnitRemoveItem(unitHandle, itemHandle);
                                dropped++;
                                foundAny = true;
                            }
                        }
                    } __except (1) {}
                }
                if (!foundAny) break;
            }
            lastDropAllCompleteTick = GetTickCount();
            dropAllActive = false;
            return dropped;
        } __except (1) {
            return 0;
        }
    }

    int DetectInventorySlot(uintptr_t (*buttonResolver)(bool, int), float wx, float wy) {
        // 1. Check CSimpleRegion bounds on button (+0x38..+0x44)
        for (int slot = 0; slot < 6; ++slot) {
            uintptr_t btn = buttonResolver ? buttonResolver(true, slot) : 0;
            if (btn && SafeIsReadable(reinterpret_cast<void*>(btn + 0x38), 16)) {
                float left = *reinterpret_cast<float*>(btn + 0x38);
                float bottom = *reinterpret_cast<float*>(btn + 0x3C);
                float right = *reinterpret_cast<float*>(btn + 0x40);
                float top = *reinterpret_cast<float*>(btn + 0x44);

                if (left > 0.0f && right > left && top > bottom &&
                    wx >= left && wx <= right && wy >= bottom && wy <= top) {
                    return slot;
                }
            }
        }

        // 2. Check alternate frame bounds (+0x88..+0x94)
        for (int slot = 0; slot < 6; ++slot) {
            uintptr_t btn = buttonResolver ? buttonResolver(true, slot) : 0;
            if (btn && SafeIsReadable(reinterpret_cast<void*>(btn + 0x88), 16)) {
                float left = *reinterpret_cast<float*>(btn + 0x88);
                float bottom = *reinterpret_cast<float*>(btn + 0x8C);
                float right = *reinterpret_cast<float*>(btn + 0x90);
                float top = *reinterpret_cast<float*>(btn + 0x94);

                if (left > 0.0f && right > left && top > bottom &&
                    wx >= left && wx <= right && wy >= bottom && wy <= top) {
                    return slot;
                }
            }
        }

        // 3. Mathematical fallback for standard Warcraft 3 1.24e Inventory UI (2 columns x 3 rows)
        // Inventory UI X: [0.500, 0.605], Y: [0.000, 0.115]
        if (wx >= 0.500f && wx <= 0.610f && wy >= 0.000f && wy <= 0.115f) {
            int col = (wx >= 0.550f) ? 1 : 0;
            int row = (wy >= 0.072f) ? 0 : ((wy >= 0.038f) ? 1 : 2);
            return row * 2 + col;
        }

        return -1;
    }

    bool TryQuickDrop(uintptr_t game, uintptr_t root, uintptr_t(*buttonResolver)(bool, int), POINT clientPt, int clientWidth, int clientHeight) {
        if (!quickDropEnabled || !game || !root || clientWidth <= 0 || clientHeight <= 0) return false;

        // Require Shift key down, and no Ctrl or Alt
        if (!(GetKeyState(VK_SHIFT) & 0x8000)) return false;
        if ((GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_MENU) & 0x8000)) return false;

        // Convert client pixel coordinates to Warcraft 3 UI space (0.0..0.8 X, 0.0..0.6 Y, Y inverted)
        float wx = (static_cast<float>(clientPt.x) / static_cast<float>(clientWidth)) * 0.8f;
        float wy = 0.6f - (static_cast<float>(clientPt.y) / static_cast<float>(clientHeight)) * 0.6f;

        int slot = DetectInventorySlot(buttonResolver, wx, wy);
        if (slot >= 0 && slot < 6) {
            __try {
                uintptr_t panel = *reinterpret_cast<uintptr_t*>(root + 0x3f4);
                if (!panel) return false;
                uintptr_t unit = *reinterpret_cast<uintptr_t*>(panel + 0x238);
                if (!unit) return false;

                return DropItemFromSlot(game, unit, slot);
            } __except (1) {
                return false;
            }
        }
        return false;
    }


    uintptr_t ResolveHeroUnit(uintptr_t root, uintptr_t game = 0) {
        // Resolve GameRoot and ConsoleUI
        uintptr_t gameRoot = 0;
        uintptr_t consoleUI = 0;
        if (game) {
            uintptr_t rootRva = ActiveGameOffsets().root;
            uintptr_t recRva = ActiveGameOffsets().recipeRoot;
            if (SafeIsReadable(reinterpret_cast<void*>(game + rootRva), 4)) gameRoot = *reinterpret_cast<uintptr_t*>(game + rootRva);
            if (SafeIsReadable(reinterpret_cast<void*>(game + recRva), 4)) consoleUI = *reinterpret_cast<uintptr_t*>(game + recRva);
        }
        if (!gameRoot) gameRoot = root;
        if (!consoleUI) consoleUI = root;

        // 0. Primary: ETS.mix exact memory path (100735c0 + 10073790)
        if (game) {
            __try {
                uintptr_t wRva = ActiveGameOffsets().world;
                if (SafeIsReadable(reinterpret_cast<void*>(game + wRva), 4)) {
                    uintptr_t pGlobal = *reinterpret_cast<uintptr_t*>(game + wRva);
                    if (pGlobal && SafeIsReadable(reinterpret_cast<void*>(pGlobal + 0x28), 2)) {
                        uint16_t playerIndex = *reinterpret_cast<uint16_t*>(pGlobal + 0x28);
                        if (playerIndex < 16 && SafeIsReadable(reinterpret_cast<void*>(pGlobal + 0x58 + playerIndex * 4), 4)) {
                            uintptr_t p1 = *reinterpret_cast<uintptr_t*>(pGlobal + 0x58 + playerIndex * 4);
                            if (p1 && SafeIsReadable(reinterpret_cast<void*>(p1 + 0x34), 4)) {
                                uintptr_t p2 = *reinterpret_cast<uintptr_t*>(p1 + 0x34);
                                if (p2 && SafeIsReadable(reinterpret_cast<void*>(p2 + 0x1e0), 4)) {
                                    uintptr_t heroUnitPtr = *reinterpret_cast<uintptr_t*>(p2 + 0x1e0);
                                    if (heroUnitPtr && SafeIsReadable(reinterpret_cast<void*>(heroUnitPtr + 0x88), 4)) {
                                        float hp = *reinterpret_cast<float*>(heroUnitPtr + 0x88);
                                        if (hp > 0.0f) return heroUnitPtr;
                                    }
                                }
                            }
                        }
                    }
                }
            } __except (1) {}
        }

        // 1. Check currently selected unit in selection manager (gameRoot + 0x3bc)
        __try {
            if (gameRoot && SafeIsReadable(reinterpret_cast<void*>(gameRoot + 0x3bc), 4)) {
                uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(gameRoot + 0x3bc);
                if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                    int count = *reinterpret_cast<int*>(selMgr + 0x28);
                    uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                    if (count > 0 && list && SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                        uintptr_t selUnit = *reinterpret_cast<uintptr_t*>(list);
                        if (selUnit && SafeIsReadable(reinterpret_cast<void*>(selUnit + 0x88), 4)) {
                            float hp = *reinterpret_cast<float*>(selUnit + 0x88);
                            if (hp > 0.0f) return selUnit;
                        }
                    }
                }
            }
        } __except (1) {}

        // 2. Try selected unit in console panel (consoleUI + 0x3f4)
        __try {
            if (consoleUI && SafeIsReadable(reinterpret_cast<void*>(consoleUI + 0x3f4), 4)) {
                uintptr_t panel = *reinterpret_cast<uintptr_t*>(consoleUI + 0x3f4);
                if (panel && SafeIsReadable(reinterpret_cast<void*>(panel + 0x238), 4)) {
                    uintptr_t unit = *reinterpret_cast<uintptr_t*>(panel + 0x238);
                    if (unit && SafeIsReadable(reinterpret_cast<void*>(unit + 0x88), 4)) {
                        float hp = *reinterpret_cast<float*>(unit + 0x88);
                        if (hp > 0.0f) return unit;
                    }
                }
            }
        } __except (1) {}

        // 3. Fallback to main hero bar (slot 0) (consoleUI + 0x3dc)
        __try {
            if (consoleUI && SafeIsReadable(reinterpret_cast<void*>(consoleUI + 0x3dc), 4)) {
                uintptr_t p = *reinterpret_cast<uintptr_t*>(consoleUI + 0x3dc);
                if (p && SafeIsReadable(reinterpret_cast<void*>(p + 0x154), 4)) {
                    p = *reinterpret_cast<uintptr_t*>(p + 0x154);
                    if (p && SafeIsReadable(reinterpret_cast<void*>(p + 8), 4)) {
                        p = *reinterpret_cast<uintptr_t*>(p + 8);
                        if (p && SafeIsReadable(reinterpret_cast<void*>(p), 4)) {
                            p = *reinterpret_cast<uintptr_t*>(p);
                            if (p && SafeIsReadable(reinterpret_cast<void*>(p + 0x1c4), 4)) {
                                uintptr_t unit = *reinterpret_cast<uintptr_t*>(p + 0x1c4);
                                if (unit && SafeIsReadable(reinterpret_cast<void*>(unit + 0x88), 4)) {
                                    float hp = *reinterpret_cast<float*>(unit + 0x88);
                                    if (hp > 0.0f) return unit;
                                }
                            }
                        }
                    }
                }
            }
        } __except (1) {}

        return 0;
    }

    uint32_t FindHeroHandle(uintptr_t game, uintptr_t root) {
        if (!game) return 0;
        // Method 0: Direct memory pointer via ResolveHeroUnit -> GetUnitHandle (exact ETS.mix parity)
        __try {
            uintptr_t u = ResolveHeroUnit(root, game);
            if (u) {
                uint32_t h = GetUnitHandle(game, u);
                if (h) return h;
            }
        } __except (1) {}

        __try {
            // Method 1: JASS GroupEnumUnitsOfPlayer(grp, GetLocalPlayer(), 0) fallback
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(LookupJassNative(game, "GetLocalPlayer"));
            auto fnCreateGroup = reinterpret_cast<uint32_t(__cdecl*)()>(LookupJassNative(game, "CreateGroup"));
            auto fnDestroyGroup = reinterpret_cast<void(__cdecl*)(uint32_t)>(LookupJassNative(game, "DestroyGroup"));
            auto fnEnumPlayer = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t, uint32_t)>(LookupJassNative(game, "GroupEnumUnitsOfPlayer"));
            auto fnFirstOfGroup = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(LookupJassNative(game, "FirstOfGroup"));
            auto fnGroupRemove = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(LookupJassNative(game, "GroupRemoveUnit"));
            auto fnGetUnitTypeId = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(LookupJassNative(game, "GetUnitTypeId"));
            auto fnIsHeroUnitId = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(LookupJassNative(game, "IsHeroUnitId"));
            auto fnGetWidgetLife = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(LookupJassNative(game, "GetWidgetLife"));

            if (fnLocal && fnCreateGroup && fnDestroyGroup && fnEnumPlayer && fnFirstOfGroup && fnGroupRemove) {
                uint32_t lp = fnLocal();
                if (lp) {
                    uint32_t grp = fnCreateGroup();
                    if (grp) {
                        fnEnumPlayer(grp, lp, 0);
                        uint32_t bestHero = 0;
                        uint32_t fallbackUnit = 0;
                        while (true) {
                            uint32_t u = fnFirstOfGroup(grp);
                            if (!u) break;
                            fnGroupRemove(grp, u);

                            float life = 100.0f;
                            if (fnGetWidgetLife) {
                                uint32_t lBits = fnGetWidgetLife(u);
                                memcpy(&life, &lBits, 4);
                            }
                            if (life > 0.405f) {
                                bool isHero = false;
                                if (fnIsHeroUnitId && fnGetUnitTypeId) {
                                    uint32_t tid = fnGetUnitTypeId(u);
                                    if (tid && fnIsHeroUnitId(tid)) isHero = true;
                                }
                                if (isHero) {
                                    bestHero = u;
                                    break;
                                }
                                if (!fallbackUnit) fallbackUnit = u;
                            }
                        }
                        fnDestroyGroup(grp);
                        if (bestHero) return bestHero;
                        if (fallbackUnit) return fallbackUnit;
                    }
                }
            }
        } __except (1) {}

        // Method 2: ResolveHeroUnit pointer -> GetUnitHandle
        __try {
            uintptr_t u = ResolveHeroUnit(root, game);
            if (u) {
                uint32_t h = GetUnitHandle(game, u);
                if (h) return h;
            }
        } __except (1) {}

        return 0;
    }

    void TickSellAll(uintptr_t game) {
        if(sellAll.Active()) {
            const auto result=sellAll.Tick(game,GetTickCount());
            sellAllActive=sellAll.Active();
            if(!sellAllActive) {
                lastSellAllCompleteTick=GetTickCount();
                const std::string prefix=sellAll.singleItem?"|CFFFCD211Quick Sell|r: ":"|CFFFCD211Sell All Item|r: ";
                if(result==SellAll::Result::Complete) sellFeedback=prefix+"Sold "+std::to_string(sellAll.sold)+" item(s)";
                else if(result==SellAll::Result::NoShop) sellFeedback=prefix+"No shop nearby";
                else if(result==SellAll::Result::Remaining) sellFeedback=prefix+"Sold "+std::to_string(sellAll.sold)+"; "+std::to_string(sellAll.remaining)+" item(s) could not be sold";
                else sellFeedback=prefix+"Stopped";
            }
        }

    }

    void OnTick(uintptr_t game, uintptr_t /*root*/) {
        autoPickGokuCompleted = false;
        if (!game) { autoPick.Reset(); sellAll.Reset(); sellAllActive=false; return; }

        TickSellAll(game);

        // Multi-pass background DropAll worker (ETS.mix FUN_10061550 parity)
        if (dropAllActive && dropAllHeroHandle) {
            uint32_t nowDrop = GetTickCount();
            if (nowDrop - lastDropTick >= 50) {
                lastDropTick = nowDrop;
                __try {
                    auto pUnitItemInSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(LookupJassNative(game, "UnitItemInSlot"));
                    auto pUnitRemoveItemFromSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(LookupJassNative(game, "UnitRemoveItemFromSlot"));
                    auto pUnitRemoveItem = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(LookupJassNative(game, "UnitRemoveItem"));

                    if (pUnitItemInSlot) {
                        uint32_t itm = pUnitItemInSlot(dropAllHeroHandle, dropAllSlot);
                        if (itm) {
                            if (pUnitRemoveItemFromSlot) {
                                pUnitRemoveItemFromSlot(dropAllHeroHandle, dropAllSlot);
                            }
                            if (pUnitRemoveItem && pUnitItemInSlot(dropAllHeroHandle, dropAllSlot) != 0) {
                                pUnitRemoveItem(dropAllHeroHandle, itm);
                            }
                        }
                    }
                    dropAllSlot++;
                    if (dropAllSlot >= 6) {
                        bool hasRemaining = false;
                        if (pUnitItemInSlot) {
                            for (int s = 0; s < 6; ++s) {
                                if (pUnitItemInSlot(dropAllHeroHandle, s)) {
                                    hasRemaining = true;
                                    break;
                                }
                            }
                        }
                        if (hasRemaining && dropAllPasses < 5) {
                            dropAllSlot = 0;
                            dropAllPasses++;
                        } else {
                            dropAllActive = false;
                            dropAllHeroHandle = 0;
                            dropAllPasses = 0;
                            lastDropAllCompleteTick = nowDrop;
                        }
                    }
                } __except (1) {
                    dropAllActive = false;
                    dropAllHeroHandle = 0;
                    dropAllPasses = 0;
                }
            }
        }


        const auto now = GetTickCount();
        // One worker/order stream. Goku takes priority over normal Auto Pick.
        autoPickGokuCompleted = autoPick.Tick(game, (autoPickEnabled || autoPickGokuEnabled) &&
            !quickDropActive && !dropAllActive && !sellAllActive &&
            now - lastDropAllCompleteTick >= 2000 && now - lastSellAllCompleteTick >= 1000,
            autoPickGokuEnabled ? GokuItemType : 0);
    }
};

} // namespace wht
