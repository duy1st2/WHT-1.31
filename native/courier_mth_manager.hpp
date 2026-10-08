#pragma once
#include <windows.h>
#include <cstdint>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include "mth_recipe_native.hpp"

namespace wht {

class CourierMthManager {
public:
    enum CourierState {
        Idle = 0,
        Delivering = 1,
        Returning = 2
    };

    bool autoCourierEnabled = true;
    CourierState state = Idle;
    uint32_t activeCourierHandle = 0;
    uint32_t targetHeroHandle = 0;
    float courierBaseX = 0.0f;
    float courierBaseY = 0.0f;
    bool hasBasePos = false;
    uint32_t lastActionTick = 0;
    std::vector<std::string> feedbackQueue;

    // MTH configuration
    struct Route {
        float x1, y1;
        float x2, y2;
        int count;
    };

    Route chayGaRoutes[3] = {
        { -1160.0f, -358.0f, -1125.0f, -398.0f, 5 },
        { -1119.0f, -81.0f,  -1163.0f, -125.0f, 5 },
        { -882.0f,  -375.0f, -934.0f,  -388.0f, 5 }
    };

    Route chaySoiRoutes[3] = {
        { 1158.0f, -397.0f, 1206.0f, -357.0f, 5 },
        { 1160.0f, -82.0f,  1212.0f, -138.0f, 5 },
        { 948.0f,  -399.0f, 897.0f,  -330.0f, 5 }
    };

    bool mth49Enabled = true;
    bool g17Enabled = false;
    bool mthNoti = true;
    bool autoPickGoku = false;

    enum MthComboStep {
        MTH_COMBO_IDLE = 0,
        MTH_COMBO_WAIT_SHOP_1,
        MTH_COMBO_BUY_SLOTS_1,
        MTH_COMBO_WAIT_SHOP_2,
        MTH_COMBO_BUY_SLOTS_2
    };

    bool mthComboActive = false;
    MthRecipeRun exactRecipe;
    bool exactComboAllowed = false;
    MthComboStep mthComboStep = MTH_COMBO_IDLE;
    uint32_t mthComboHero = 0;
    uint32_t mthComboShop1 = 0;
    uint32_t mthComboShop2 = 0;
    std::vector<int> mthComboSlots1;
    std::vector<int> mthComboSlots2;
    size_t mthComboSlotIdx = 0;
    uint32_t mthComboNextTick = 0;
    std::string mthComboSkillName;
    int mthComboAlreadyOwned = 0;
    bool (*mthClickDirectFn)(int) = nullptr;
    int pendingComboType = -1;
    uint32_t lastApproachCheckTick = 0;

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

    static bool IsValidJassHandle(uint32_t h) {
        return h >= 0x100000 && h < 0x10000000;
    }

    uint32_t GetUnitHandle(uintptr_t game, uintptr_t unit) {
        if (!unit || !game) return 0;
        __try {
            uintptr_t umRva = ActiveGameOffsets().getUnitManager;
            uintptr_t luRva = ActiveGameOffsets().lookupUnit;
            uintptr_t wRva = ActiveGameOffsets().world;
            if (SafeIsExecutable(game + umRva) && SafeIsExecutable(game + luRva) &&
                SafeIsReadable(reinterpret_cast<void*>(game + wRva), 4)) {
                uintptr_t stateObj = *reinterpret_cast<uintptr_t*>(game + wRva);
                if (stateObj && SafeIsReadable(reinterpret_cast<void*>(stateObj), 0x20)) {
                    auto getContext = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(game + umRva);
                    uintptr_t ctx = getContext(stateObj);
                    if (ctx && SafeIsReadable(reinterpret_cast<void*>(ctx), 0x20)) {
                        auto getHandle = reinterpret_cast<uint32_t(__thiscall*)(uintptr_t, uintptr_t, int)>(game + luRva);
                        uint32_t h = getHandle(ctx, unit, 0);
                        if (IsValidJassHandle(h)) return h;
                    }
                }
            }
            return 0;
        } __except (1) {
            return 0;
        }
    }

    bool GetUnitCoords(uintptr_t game, uint32_t unitHandle, float& outX, float& outY) {
        if (!game || !IsValidJassHandle(unitHandle)) return false;
        __try {
            // Method 1: JASS native GetUnitX / GetUnitY
            auto pGetUnitX = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(LookupJassNative(game, "GetUnitX"));
            auto pGetUnitY = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(LookupJassNative(game, "GetUnitY"));
            if (pGetUnitX && pGetUnitY && SafeIsExecutable(reinterpret_cast<uintptr_t>(pGetUnitX)) && SafeIsExecutable(reinterpret_cast<uintptr_t>(pGetUnitY))) {
                uint32_t xBits = pGetUnitX(unitHandle);
                uint32_t yBits = pGetUnitY(unitHandle);
                float x = 0.0f, y = 0.0f;
                memcpy(&x, &xBits, 4);
                memcpy(&y, &yBits, 4);
                if (std::isfinite(x) && std::isfinite(y) && (x != 0.0f || y != 0.0f)) {
                    outX = x;
                    outY = y;
                    return true;
                }
            }

            // Method 2: CUnit memory read at +0x288 / +0x28C
            uintptr_t htuRva = ActiveGameOffsets().handleToUnit;
            if (SafeIsExecutable(game + htuRva)) {
                typedef uintptr_t (__thiscall *LookupUnit_t)(uint32_t handle);
                auto lookupUnit = reinterpret_cast<LookupUnit_t>(game + htuRva);
                uintptr_t u = lookupUnit(unitHandle);
                if (u && SafeIsReadable(reinterpret_cast<void*>(u + 0x288), 8)) {
                    float x = *reinterpret_cast<float*>(u + 0x288);
                    float y = *reinterpret_cast<float*>(u + 0x28c);
                    if (std::isfinite(x) && std::isfinite(y)) {
                        outX = x;
                        outY = y;
                        return true;
                    }
                }
            }
            return false;
        } __except (1) {
            return false;
        }
    }

    uint32_t FindLocalHero(uintptr_t game, void* (*jassLookup)(const char*), uintptr_t root = 0) {
        if (!game) return 0;
        auto resolveJass = [&](const char* n) { void* p = jassLookup ? jassLookup(n) : nullptr; return p ? p : LookupJassNative(game, n); };

        // Method 0: Check currently selected unit in UI (if it has hero data +0x1f4)
        if (root && SafeIsReadable(reinterpret_cast<void*>(root + 0x3bc), 4)) {
            uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(root + 0x3bc);
            if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                int count = *reinterpret_cast<int*>(selMgr + 0x28);
                uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                if (count > 0 && list && SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                    uintptr_t selUnit = *reinterpret_cast<uintptr_t*>(list);
                    if (selUnit && SafeIsReadable(reinterpret_cast<void*>(selUnit + 0x1f4), 4)) {
                        uintptr_t heroData = *reinterpret_cast<uintptr_t*>(selUnit + 0x1f4);
                        if (heroData) {
                            uint32_t h = GetUnitHandle(game, selUnit);
                            if (IsValidJassHandle(h)) return h;
                        }
                    }
                }
            }
        }

        // Method 1: JASS GroupEnumUnitsOfPlayer
        __try {
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(resolveJass("GetLocalPlayer"));
            auto fnCreateGroup = reinterpret_cast<uint32_t(__cdecl*)()>(resolveJass("CreateGroup"));
            auto fnEnumPlayer = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t, uint32_t)>(resolveJass("GroupEnumUnitsOfPlayer"));
            auto fnFirstOfGroup = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("FirstOfGroup"));
            auto fnGroupRemove = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(resolveJass("GroupRemoveUnit"));
            auto fnDestroyGroup = reinterpret_cast<void(__cdecl*)(uint32_t)>(resolveJass("DestroyGroup"));
            auto fnIsHeroUnitId = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("IsHeroUnitId"));
            auto fnGetUnitTypeId = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("GetUnitTypeId"));

            if (fnLocal && fnCreateGroup && fnEnumPlayer && fnFirstOfGroup && fnGroupRemove && fnDestroyGroup) {
                uint32_t lp = fnLocal();
                if (IsValidJassHandle(lp)) {
                    uint32_t grp = fnCreateGroup();
                    if (grp) {
                        fnEnumPlayer(grp, lp, 0);
                        uint32_t foundHero = 0;
                        while (true) {
                            uint32_t u = fnFirstOfGroup(grp);
                            if (!u) break;
                            fnGroupRemove(grp, u);

                            if (!foundHero && IsValidJassHandle(u)) {
                                bool isHero = false;
                                if (fnIsHeroUnitId && fnGetUnitTypeId && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnIsHeroUnitId))) {
                                    uint32_t tid = fnGetUnitTypeId(u);
                                    if (tid && fnIsHeroUnitId(tid)) isHero = true;
                                }
                                uintptr_t htuRva = ActiveGameOffsets().handleToUnit;
                                if (!isHero && SafeIsExecutable(game + htuRva)) {
                                    typedef uintptr_t (__thiscall *LookupUnit_t)(uint32_t handle);
                                    auto lookupUnit = reinterpret_cast<LookupUnit_t>(game + htuRva);
                                    uintptr_t uPtr = lookupUnit(u);
                                    if (uPtr && SafeIsReadable(reinterpret_cast<void*>(uPtr + 0x1f4), 4)) {
                                        if (*reinterpret_cast<uintptr_t*>(uPtr + 0x1f4) != 0) isHero = true;
                                    }
                                }
                                if (isHero) foundHero = u;
                            }
                        }
                        fnDestroyGroup(grp);
                        if (foundHero) return foundHero;
                    }
                }
            }
        } __except (1) {}

        // Method 2: ETS.mix exact memory path (100735c0 + 10073790) converted to handle
        __try {
            if (SafeIsReadable(reinterpret_cast<void*>(game + ActiveGameOffsets().world), 4)) {
                uintptr_t pGlobal = *reinterpret_cast<uintptr_t*>(game + ActiveGameOffsets().world);
                if (pGlobal && SafeIsReadable(reinterpret_cast<void*>(pGlobal + 0x28), 2)) {
                    uint16_t playerIndex = *reinterpret_cast<uint16_t*>(pGlobal + 0x28);
                    if (playerIndex < 16 && SafeIsReadable(reinterpret_cast<void*>(pGlobal + 0x58 + playerIndex * 4), 4)) {
                        uintptr_t p1 = *reinterpret_cast<uintptr_t*>(pGlobal + 0x58 + playerIndex * 4);
                        if (p1 && SafeIsReadable(reinterpret_cast<void*>(p1 + 0x34), 4)) {
                            uintptr_t p2 = *reinterpret_cast<uintptr_t*>(p1 + 0x34);
                            if (p2 && SafeIsReadable(reinterpret_cast<void*>(p2 + 0x1e0), 4)) {
                                uintptr_t heroUnitPtr = *reinterpret_cast<uintptr_t*>(p2 + 0x1e0);
                                if (heroUnitPtr) {
                                    uint32_t h = GetUnitHandle(game, heroUnitPtr);
                                    if (IsValidJassHandle(h)) return h;
                                }
                            }
                        }
                    }
                }
            }
        } __except (1) {}

        return 0;
    }

    uint32_t FindCourier(uintptr_t game, uintptr_t root, void* (*jassLookup)(const char*)) {
        if (activeCourierHandle) return activeCourierHandle;
        if (!game || !jassLookup) return 0;

        __try {
            // Check currently selected unit first
            if (root && SafeIsReadable(reinterpret_cast<void*>(root + 0x3bc), 4)) {
                uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(root + 0x3bc);
                if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                    int count = *reinterpret_cast<int*>(selMgr + 0x28);
                    uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                    if (count > 0 && list && SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                        uintptr_t selUnit = *reinterpret_cast<uintptr_t*>(list);
                        if (selUnit && SafeIsReadable(reinterpret_cast<void*>(selUnit + 0x1f4), 4)) {
                            uintptr_t heroData = *reinterpret_cast<uintptr_t*>(selUnit + 0x1f4);
                            if (!heroData) {
                                // Non-hero unit selected! Good candidate for courier
                                uint32_t h = GetUnitHandle(game, selUnit);
                                if (h) {
                                    activeCourierHandle = h;
                                    return h;
                                }
                            }
                        }
                    }
                }
            }

            // Fallback: enum units of player
            auto resolveJass = [&](const char* n) { void* p = jassLookup ? jassLookup(n) : nullptr; return p ? p : LookupJassNative(game, n); };
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(resolveJass("GetLocalPlayer"));
            auto fnCreateGroup = reinterpret_cast<uint32_t(__cdecl*)()>(resolveJass("CreateGroup"));
            auto fnEnumPlayer = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t, uint32_t)>(resolveJass("GroupEnumUnitsOfPlayer"));
            auto fnFirstOfGroup = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("FirstOfGroup"));
            auto fnGroupRemove = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(resolveJass("GroupRemoveUnit"));
            auto fnDestroyGroup = reinterpret_cast<void(__cdecl*)(uint32_t)>(resolveJass("DestroyGroup"));
            auto fnUnitItemInSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(resolveJass("UnitItemInSlot"));
            uintptr_t htuRva = ActiveGameOffsets().handleToUnit;
            auto lookupUnit = SafeIsExecutable(game + htuRva) ? reinterpret_cast<uintptr_t(__thiscall*)(uint32_t)>(game + htuRva) : nullptr;

            if (!fnLocal || !fnCreateGroup || !fnEnumPlayer || !fnFirstOfGroup || !fnGroupRemove || !fnDestroyGroup) return 0;

            uint32_t lp = fnLocal();
            if (!lp) return 0;

            uint32_t grp = fnCreateGroup();
            if (!grp) return 0;

            fnEnumPlayer(grp, lp, 0);
            uint32_t courier = 0;
            uint32_t nonHeroUnit = 0;

            while (true) {
                uint32_t u = fnFirstOfGroup(grp);
                if (!u) break;
                fnGroupRemove(grp, u);

                // Priority 1: unit with item in inventory
                if (!courier && fnUnitItemInSlot) {
                    for (int s = 0; s < 6; ++s) {
                        if (fnUnitItemInSlot(u, s)) {
                            courier = u;
                            break;
                        }
                    }
                }

                // Priority 2: mobile non-hero unit
                if (!nonHeroUnit && lookupUnit) {
                    uintptr_t cunit = lookupUnit(u);
                    if (cunit && SafeIsReadable(reinterpret_cast<void*>(cunit + 0x1f4), 4)) {
                        uintptr_t heroData = *reinterpret_cast<uintptr_t*>(cunit + 0x1f4);
                        if (!heroData) {
                            if (SafeIsReadable(reinterpret_cast<void*>(cunit + 0x5c), 4)) {
                                uint32_t flags = *reinterpret_cast<uint32_t*>(cunit + 0x5c);
                                if (!(flags & 0x40)) { // not a structure/building
                                    float hp = 0.0f;
                                    if (SafeIsReadable(reinterpret_cast<void*>(cunit + 0x88), 4)) {
                                        hp = *reinterpret_cast<float*>(cunit + 0x88);
                                    }
                                    if (hp > 0.0f) {
                                        nonHeroUnit = u;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            fnDestroyGroup(grp);

            uint32_t finalCourier = courier ? courier : nonHeroUnit;
            if (finalCourier) activeCourierHandle = finalCourier;
            return finalCourier;
        } __except (1) {
            return 0;
        }
    }

    bool OrderDeliver(uintptr_t game, uintptr_t root, void* (*jassLookup)(const char*), std::wstring& outMsg) {
        if (!game || !jassLookup) {
            outMsg = L"Game engine not ready";
            return false;
        }

        uint32_t courier = FindCourier(game, root, jassLookup);
        if (!courier) {
            outMsg = L"Khong tim thay Ga/Courier cua ban (hay chon Ga va go #setga)";
            return false;
        }
        activeCourierHandle = courier;

        // Remember base position if not set
        if (!hasBasePos) {
            float cx = 0.0f, cy = 0.0f;
            if (GetUnitCoords(game, courier, cx, cy)) {
                courierBaseX = cx;
                courierBaseY = cy;
                hasBasePos = true;
            }
        }

        uint32_t hero = FindLocalHero(game, jassLookup);
        if (!hero || hero == courier) {
            outMsg = L"Khong tim thay Hero de chuyen do";
            return false;
        }
        targetHeroHandle = hero;

        // Check if courier has items
        auto fnUnitItemInSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(jassLookup("UnitItemInSlot"));
        int itemsOnCourier = 0;
        if (fnUnitItemInSlot) {
            for (int s = 0; s < 6; ++s) {
                if (fnUnitItemInSlot(courier, s)) itemsOnCourier++;
            }
        }

        // Try speed boost ability ('A01S' or similar order 0xD00A4 / order "speed")
        auto fnIssueImmediateOrderById = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(jassLookup("IssueImmediateOrderById"));
        if (fnIssueImmediateOrderById && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnIssueImmediateOrderById))) {
            fnIssueImmediateOrderById(courier, 0xD00A4);
        }

        // Issue smart order to hero (0xD0003)
        auto fnIssueTargetOrderById = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int, uint32_t)>(jassLookup("IssueTargetOrderById"));
        if (fnIssueTargetOrderById && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnIssueTargetOrderById))) {
            fnIssueTargetOrderById(courier, 0xD0003, hero);
        }

        state = Delivering;
        lastActionTick = GetTickCount();

        outMsg = L"Ga dang chuyen " + std::to_wstring(itemsOnCourier) + L" do ve Hero!";
        feedbackQueue.push_back("|cfffEBA0E[Auto Ga]:|r Dang chuyen do ve Hero...");
        return true;
    }

    bool OrderReturn(uintptr_t game, void* (*jassLookup)(const char*), std::wstring& outMsg) {
        if (!game || !jassLookup || !activeCourierHandle) {
            outMsg = L"Chua xac dinh Ga (go #ga truoc)";
            return false;
        }

        auto fnIssuePointOrderById = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int, const float*, const float*)>(jassLookup("IssuePointOrderById"));
        if (fnIssuePointOrderById && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnIssuePointOrderById))) {
            fnIssuePointOrderById(activeCourierHandle, 0xD0012 /* move */, &courierBaseX, &courierBaseY);
            state = Returning;
            outMsg = L"Ga dang quay ve can cu!";
            feedbackQueue.push_back("|cfffEBA0E[Auto Ga]:|r Ga dang quay ve can cu!");
            return true;
        }
        return false;
    }

    // Active MTH route state machine
    bool mthActive = false;
    uint32_t activeMthUnit = 0;
    int mthRouteType = 0; // 0 = Ga, 1 = Soi
    int mthRouteIndex = 0; // 0, 1, 2
    int mthLapsTotal = 5;
    int mthLapsRemaining = 0;
    int mthTargetPoint = 0; // 0 = to Point 1 (x1, y1), 1 = to Point 2 (x2, y2)
    uint32_t lastMthOrderTick = 0;
    float lastMthUx = 0.0f;
    float lastMthUy = 0.0f;
    int mthStallTicks = 0;

    void StopMth() {
        mthActive = false;
        activeMthUnit = 0;
        feedbackQueue.push_back("|cfffEBA0E[MTH]:|r Da dung chay Ga/Soi.");
    }

    inline void WorldIssueImmediateOrder(uintptr_t game, uintptr_t root, uint32_t orderId) {
        if (!game || !root) return;
        __try {
            if (!SafeIsReadable(reinterpret_cast<void*>(root + 0x1B4), 4)) return;
            uintptr_t pWorld = *reinterpret_cast<uintptr_t*>(root + 0x1B4);
            if (!pWorld) return;
            uintptr_t fn = game + ActiveGameOffsets().issueImmediateOrder;
            if (!SafeIsExecutable(fn)) return;
            __asm {
                mov ecx, pWorld
                push 4
                push 1
                push 0
                push orderId
                call fn
            }
        } __except (1) {}
    }

    inline void WorldIssuePointOrder(uintptr_t game, uintptr_t root, uint32_t orderId, float x, float y, int queue) {
        if (!game || !root) return;
        __try {
            if (!SafeIsReadable(reinterpret_cast<void*>(root + 0x1B4), 4)) return;
            uintptr_t pWorld = *reinterpret_cast<uintptr_t*>(root + 0x1B4);
            if (!pWorld) return;
            uintptr_t fn = game + ActiveGameOffsets().issuePointOrder;
            if (!SafeIsExecutable(fn)) return;
            uint32_t ux = *reinterpret_cast<uint32_t*>(&x);
            uint32_t uy = *reinterpret_cast<uint32_t*>(&y);
            __asm {
                mov ecx, pWorld
                push queue
                push -1
                push 0
                push uy
                push ux
                push 0
                push orderId
                call fn
            }
        } __except (1) {}
    }

    bool ExecuteMthRoute(int routeType /* 0=Ga, 1=Soi */, int index /* 0, 1, 2 */, uintptr_t game, uintptr_t root, void* (*jassLookup)(const char*), std::wstring& outMsg) {
        if (!game || !jassLookup || index < 0 || index > 2) {
            outMsg = L"Tuyen duong khong hop le";
            return false;
        }

        const Route& r = (routeType == 0) ? chayGaRoutes[index] : chaySoiRoutes[index];
        const wchar_t* label = (routeType == 0) ? L"Chay Ga" : L"Chay Soi";

        // Check currently selected unit in UI
        uintptr_t curSelUnit = 0;
        uint32_t curSelHandle = 0;
        bool curIsHero = false;
        if (root && SafeIsReadable(reinterpret_cast<void*>(root + 0x3bc), 4)) {
            uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(root + 0x3bc);
            if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                int count = *reinterpret_cast<int*>(selMgr + 0x28);
                uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                if (count > 0 && list && SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                    curSelUnit = *reinterpret_cast<uintptr_t*>(list);
                    if (curSelUnit) {
                        curSelHandle = GetUnitHandle(game, curSelUnit);
                        if (SafeIsReadable(reinterpret_cast<void*>(curSelUnit + 0x1f4), 4)) {
                            curIsHero = (*reinterpret_cast<uintptr_t*>(curSelUnit + 0x1f4) != 0);
                        }
                    }
                }
            }
        }

        // Determine courier handle
        uint32_t courierHandle = 0;
        if (curSelHandle && !curIsHero) {
            // Player already selected non-hero unit (Courier)
            courierHandle = curSelHandle;
            activeCourierHandle = courierHandle;
        } else {
            // Find courier among player units
            courierHandle = FindCourier(game, root, jassLookup);
        }

        if (!courierHandle) {
            outMsg = L"Hay chon unit (Ga hoac Soi) truoc khi ra lenh!";
            feedbackQueue.push_back("|cffff5c5c[MTH]:|r Hay chon unit (Ga hoac Soi) truoc khi ra lenh!");
            return false;
        }

        auto fnClearSelection = reinterpret_cast<void(__cdecl*)()>(jassLookup("ClearSelection"));
        auto fnSelectUnit = reinterpret_cast<void(__cdecl*)(uint32_t, int)>(jassLookup("SelectUnit"));

        // If the player did not have the courier selected, select it now so CWorld orders apply to it
        bool switchedSelection = false;
        if (curSelHandle != courierHandle && fnClearSelection && fnSelectUnit) {
            fnClearSelection();
            fnSelectUnit(courierHandle, 1);
            switchedSelection = true;
        }

        int count = (r.count > 0 && r.count <= 20) ? r.count : 5;

        // Check if CWorld is available for native queued orders (exact ETS FUN_1003e920 behavior)
        uintptr_t pWorld = (root && SafeIsReadable(reinterpret_cast<void*>(root + 0x1B4), 4))
            ? *reinterpret_cast<uintptr_t*>(root + 0x1B4) : 0;

        if (pWorld) {
            // 1. Clear previous orders on courier
            WorldIssueImmediateOrder(game, root, 0xD0004);

            // 2. Queue all back-and-forth movement laps natively into the engine order queue
            for (int i = 0; i < count; ++i) {
                WorldIssuePointOrder(game, root, 0xD0012, r.x1, r.y1, 1);
                WorldIssuePointOrder(game, root, 0xD0012, r.x2, r.y2, 1);
            }

            // Stop manual polling loop so it doesn't interrupt the engine's native queue
            mthActive = false;
            activeMthUnit = 0;
        } else {
            // Fallback for mock/test environment without CWorld
            auto fnStop = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(jassLookup("IssueImmediateOrderById"));
            auto fnMove = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int, const float*, const float*)>(jassLookup("IssuePointOrderById"));
            if (fnStop && fnMove && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnStop)) && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnMove))) {
                fnStop(courierHandle, 0xD0004);
                fnMove(courierHandle, 0xD0012, &r.x1, &r.y1);
            }
        }

        // If we switched selection from Hero to Courier, restore player's Hero selection
        if (switchedSelection && curIsHero && curSelHandle && fnClearSelection && fnSelectUnit) {
            fnClearSelection();
            fnSelectUnit(curSelHandle, 1);
        }

        outMsg = std::wstring(label) + L" " + std::to_wstring(index + 1) + L" bat dau (" + std::to_wstring(count) + L" luot qua lai)!";
        feedbackQueue.push_back("|cfffEBA0E[MTH]:|r " + Utf8(outMsg));
        return true;
    }

    void Update(uintptr_t game, void* (*jassLookup)(const char*)) {
        if (!game || !jassLookup) return;
        uint32_t now = GetTickCount();
        if (now - lastActionTick < 200) return; // 5Hz tick
        lastActionTick = now;

        // 1. Active MTH Route back-and-forth processing
        if (mthActive && activeMthUnit) {
            float ux = 0.0f, uy = 0.0f;
            if (GetUnitCoords(game, activeMthUnit, ux, uy)) {
                const Route& r = (mthRouteType == 0) ? chayGaRoutes[mthRouteIndex] : chaySoiRoutes[mthRouteIndex];
                float tx = (mthTargetPoint == 0) ? r.x1 : r.x2;
                float ty = (mthTargetPoint == 0) ? r.y1 : r.y2;

                float dx = ux - tx;
                float dy = uy - ty;
                float distSq = dx * dx + dy * dy;

                float moveDelta = std::abs(ux - lastMthUx) + std::abs(uy - lastMthUy);
                lastMthUx = ux;
                lastMthUy = uy;
                if (moveDelta < 1.0f) {
                    mthStallTicks++;
                } else {
                    mthStallTicks = 0;
                }

                auto fnMove = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int, const float*, const float*)>(jassLookup("IssuePointOrderById"));

                // Arrival criteria:
                // Distance <= 35.0f OR distance <= 55.0f with unit stopped for >= 2 ticks
                bool arrived = (distSq <= 35.0f * 35.0f) || (distSq <= 55.0f * 55.0f && mthStallTicks >= 2);

                if (arrived) {
                    mthStallTicks = 0;
                    if (mthTargetPoint == 0) {
                        // Arrived at Point 1! Turn and move to Point 2
                        mthTargetPoint = 1;
                        if (fnMove && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnMove))) {
                            fnMove(activeMthUnit, 0xD0012, &r.x2, &r.y2);
                        }
                        lastMthOrderTick = now;
                    } else {
                        // Arrived at Point 2! Completed 1 round-trip lap!
                        mthLapsRemaining--;
                        int completedLap = mthLapsTotal - mthLapsRemaining;
                        if (mthLapsRemaining > 0) {
                            mthTargetPoint = 0;
                            if (fnMove && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnMove))) {
                                fnMove(activeMthUnit, 0xD0012, &r.x1, &r.y1);
                            }
                            lastMthOrderTick = now;
                            feedbackQueue.push_back("|cfffEBA0E[MTH]:|r Luot " + std::to_string(completedLap) + "/" + std::to_string(mthLapsTotal) + " xong! Dang quay lai...");
                        } else {
                            // All laps finished!
                            mthActive = false;
                            feedbackQueue.push_back("|cfffEBA0E[MTH]:|r Da hoan thanh xong toan bo " + std::to_string(mthLapsTotal) + " luot chay qua lai!");
                        }
                    }
                } else {
                    // Periodic re-assert order if stalled or every 3 seconds to keep moving
                    if (now - lastMthOrderTick > 3000 || mthStallTicks >= 4) {
                        if (fnMove && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnMove))) {
                            fnMove(activeMthUnit, 0xD0012, &tx, &ty);
                        }
                        lastMthOrderTick = now;
                        mthStallTicks = 0;
                    }
                }
            } else {
                mthActive = false;
            }
        }

        // 2. Active Courier Delivery processing
        if (state == Delivering && activeCourierHandle && targetHeroHandle) {
            float cx = 0.0f, cy = 0.0f, hx = 0.0f, hy = 0.0f;
            if (GetUnitCoords(game, activeCourierHandle, cx, cy) && GetUnitCoords(game, targetHeroHandle, hx, hy)) {
                float dx = cx - hx;
                float dy = cy - hy;
                float distSq = dx * dx + dy * dy;

                // When courier is within range (300 units) of hero
                if (distSq <= 300.0f * 300.0f) {
                    auto fnUnitItemInSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(jassLookup("UnitItemInSlot"));
                    auto fnUnitDropItemTarget = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, uint32_t, uint32_t)>(jassLookup("UnitDropItemTarget"));
                    auto fnUnitRemoveFromSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(jassLookup("UnitRemoveItemFromSlot"));
                    auto fnUnitAddItem = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, uint32_t)>(jassLookup("UnitAddItem"));

                    int transferred = 0;
                    if (fnUnitItemInSlot) {
                        for (int s = 0; s < 6; ++s) {
                            uint32_t itm = fnUnitItemInSlot(activeCourierHandle, s);
                            if (itm) {
                                // Transfer to hero
                                if (fnUnitRemoveFromSlot && fnUnitAddItem &&
                                    SafeIsExecutable(reinterpret_cast<uintptr_t>(fnUnitRemoveFromSlot)) &&
                                    SafeIsExecutable(reinterpret_cast<uintptr_t>(fnUnitAddItem))) {
                                    uint32_t removed = fnUnitRemoveFromSlot(activeCourierHandle, s);
                                    if (removed) {
                                        fnUnitAddItem(targetHeroHandle, removed);
                                        transferred++;
                                    }
                                } else if (fnUnitDropItemTarget && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnUnitDropItemTarget))) {
                                    fnUnitDropItemTarget(activeCourierHandle, itm, targetHeroHandle);
                                    transferred++;
                                }
                            }
                        }
                    }

                    // Return to base after delivery
                    state = Returning;
                    auto fnIssuePointOrderById = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int, const float*, const float*)>(jassLookup("IssuePointOrderById"));
                    if (fnIssuePointOrderById && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnIssuePointOrderById)) && hasBasePos) {
                        fnIssuePointOrderById(activeCourierHandle, 0xD0012, &courierBaseX, &courierBaseY);
                    }
                    feedbackQueue.push_back("|cfffEBA0E[Auto Ga]:|r Da chuyen " + std::to_string(transferred) + " do xong! Ga dang quay ve.");
                }
            }
        } else if (state == Returning && activeCourierHandle && hasBasePos) {
            float cx = 0.0f, cy = 0.0f;
            if (GetUnitCoords(game, activeCourierHandle, cx, cy)) {
                float dx = cx - courierBaseX;
                float dy = cy - courierBaseY;
                if ((dx * dx + dy * dy) <= 150.0f * 150.0f) {
                    state = Idle;
                    feedbackQueue.push_back("|cfffEBA0E[Auto Ga]:|r Ga da ve den can cu an toan.");
                }
            }
        }
    }

    void Update(uintptr_t game, uintptr_t root, void* (*jassLookup)(const char*)) {
        Update(game, jassLookup);
        // Auto Pick Goku now uses ItemManager's synchronized AutoPick worker.

        if (exactRecipe.Active()) {
            MthRecipeNative api(game, root, jassLookup, mthClickDirectFn);
            const auto result = TickMthRecipe(exactRecipe, api,
                exactComboAllowed && (mth49Enabled || g17Enabled), GetTickCount());
            mthComboActive = exactRecipe.Active();
            if (!mthComboActive) ReportExactRecipe(result);
            return;
        }

        // 4. MTH Combo Quick Buy state machine (Cadence 100ms chuẩn xác như ETS.mix)
        if (mthComboActive) {
            uint32_t now = GetTickCount();
            if (now >= mthComboNextTick) {
                auto resolveJass = [&](const char* n) { void* p = jassLookup ? jassLookup(n) : nullptr; return p ? p : LookupJassNative(game, n); };
                auto fnClearSelection = reinterpret_cast<void(__cdecl*)()>(resolveJass("ClearSelection"));
                auto fnSelectUnit = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(resolveJass("SelectUnit"));

                if (mthComboStep == MTH_COMBO_WAIT_SHOP_1) {
                    // CCommandBar của Shop 1 đã được engine dựng xong đầy đủ các slot sau 100ms!
                    if (mthClickDirectFn) {
                        for (int slot : mthComboSlots1) {
                            mthClickDirectFn(slot);
                        }
                    }

                    if (mthComboShop2 && !mthComboSlots2.empty() && fnClearSelection && fnSelectUnit) {
                        // Chọn tiếp Shop 2 (Nhà Ghép 'n018') để ghép đồ
                        fnClearSelection();
                        fnSelectUnit(mthComboShop2, 1);
                        mthComboStep = MTH_COMBO_WAIT_SHOP_2;
                        mthComboNextTick = now + 100; // 100ms delay để engine nạp CCommandBar của Shop 2
                    } else {
                        // Không cần ghép (hoặc không có Shop 2), khôi phục lại Hero ngay lập tức
                        if (fnClearSelection && fnSelectUnit && mthComboHero) {
                            fnClearSelection();
                            fnSelectUnit(mthComboHero, 1);
                        }
                        mthComboActive = false;
                        mthComboStep = MTH_COMBO_IDLE;
                        if (mthNoti) {
                            if (mthComboSlots1.empty()) {
                                feedbackQueue.push_back("|cfffEBA0E[MTH]|r Da co du nguyen lieu, tu dong ep " + mthComboSkillName + " xong!");
                            } else if (mthComboAlreadyOwned > 0) {
                                feedbackQueue.push_back("|cfffEBA0E[MTH]|r Da mua " + std::to_string(mthComboSlots1.size()) + " mon con thieu & ep " + mthComboSkillName + " xong!");
                            } else {
                                feedbackQueue.push_back("|cfffEBA0E[MTH]|r Da tu dong mua & ep " + mthComboSkillName + " xong!");
                            }
                        }
                    }
                } else if (mthComboStep == MTH_COMBO_WAIT_SHOP_2) {
                    // CCommandBar của Shop 2 (Nhà Ghép) đã sẵn sàng!
                    if (mthClickDirectFn) {
                        for (int slot : mthComboSlots2) {
                            mthClickDirectFn(slot);
                        }
                    }

                    // Hoàn tất ghép đồ -> lập tức khôi phục lại chọn Hero cho người chơi
                    if (fnClearSelection && fnSelectUnit && mthComboHero) {
                        fnClearSelection();
                        fnSelectUnit(mthComboHero, 1);
                    }
                    mthComboActive = false;
                    mthComboStep = MTH_COMBO_IDLE;
                    if (mthNoti) {
                        if (mthComboSlots1.empty()) {
                            feedbackQueue.push_back("|cfffEBA0E[MTH]|r Da co du nguyen lieu, tu dong ep " + mthComboSkillName + " xong!");
                        } else if (mthComboAlreadyOwned > 0) {
                            feedbackQueue.push_back("|cfffEBA0E[MTH]|r Da mua " + std::to_string(mthComboSlots1.size()) + " mon con thieu & ep " + mthComboSkillName + " xong!");
                        } else {
                            feedbackQueue.push_back("|cfffEBA0E[MTH]|r Da tu dong mua & ep " + mthComboSkillName + " xong!");
                        }
                    }
                }
            }
        }


        // 5. Auto Buy when Hero reaches near shop (ETS.mix behaviour: when approaching shop <= 950 range, auto trigger purchase)
        if (pendingComboType >= 0 && !mthComboActive && jassLookup) {
            uint32_t now = GetTickCount();
            if (now - lastApproachCheckTick >= 80) {
                lastApproachCheckTick = now;
                uint32_t hero = FindLocalHero(game, jassLookup, root);
                if (hero) {
                    uint32_t checkShopCode = (pendingComboType == 0) ? 0x6E303035 : 0x68303430;
                    uint32_t nearShop = FindShopUnit(game, hero, checkShopCode, jassLookup, 950.0f, root);
                    if (!nearShop && checkShopCode == 0x6E303035) {
                        nearShop = FindShopUnit(game, hero, 0x68303430, jassLookup, 950.0f, root);
                    }
                    if (nearShop) {
                        int cType = pendingComboType;
                        pendingComboType = -1;
                        ExecuteMthCombo(game, root, jassLookup, mthClickDirectFn, nullptr, cType);
                    }
                }
            }
        }
    }

    bool SetAutoPickGoku(bool enabled) {
        if (enabled && !mth49Enabled && !g17Enabled) {
            feedbackQueue.push_back("|cfffEBA0E[MTH]|r Ban chua bat");
            return false;
        }
        if (autoPickGoku == enabled) return true;
        autoPickGoku = enabled;
        if (mthNoti) {
            feedbackQueue.push_back(autoPickGoku ? "|cfffEBA0E[MTH]|r Auto Pick Goku: |cff44ff3dOn"
                                                 : "|cfffEBA0E[MTH]|r Auto Pick Goku: |cffff5c5cOff");
        }
        return true;
    }

    void ToggleAutoPickGoku() { SetAutoPickGoku(!autoPickGoku); }

    void CompleteAutoPickGoku() {
        if (!autoPickGoku) return;
        autoPickGoku = false;
        if (mthNoti) feedbackQueue.push_back("|cfffEBA0E[MTH]|r Da co Goku trong tui, Auto Pick Goku: Off");
    }

    uint32_t FindShopUnit(uintptr_t game, uint32_t hero, uint32_t targetTypeId, void* (*jassLookup)(const char*), float radius = 3500.0f, uintptr_t root = 0, uint32_t excludeUnit = 0) {
        if (!game) return 0;
        auto resolveJass = [&](const char* n) { void* p = jassLookup ? jassLookup(n) : nullptr; return p ? p : LookupJassNative(game, n); };

        auto fnGetUnitTypeId = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("GetUnitTypeId"));
        auto fnCreateGroup = reinterpret_cast<uint32_t(__cdecl*)()>(resolveJass("CreateGroup"));
        auto fnDestroyGroup = reinterpret_cast<void(__cdecl*)(uint32_t)>(resolveJass("DestroyGroup"));
        auto fnGroupEnumUnitsInRange = reinterpret_cast<void(__cdecl*)(uint32_t, const float*, const float*, const float*, uint32_t)>(resolveJass("GroupEnumUnitsInRange"));
        auto fnFirstOfGroup = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("FirstOfGroup"));
        auto fnGroupRemoveUnit = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(resolveJass("GroupRemoveUnit"));
        auto fnPlayer = reinterpret_cast<uint32_t(__cdecl*)(int)>(resolveJass("Player"));
        auto fnEnumPlayer = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t, uint32_t)>(resolveJass("GroupEnumUnitsOfPlayer"));

        if (!fnCreateGroup || !fnDestroyGroup || !fnFirstOfGroup || !fnGroupRemoveUnit || !fnGetUnitTypeId ||
            !SafeIsExecutable(reinterpret_cast<uintptr_t>(fnCreateGroup)) ||
            !SafeIsExecutable(reinterpret_cast<uintptr_t>(fnGetUnitTypeId))) {
            return 0;
        }

        float hx = 0.0f, hy = 0.0f;
        bool coordsOk = hero ? GetUnitCoords(game, hero, hx, hy) : false;

        // Method 0: If the player currently has the shop selected in UI, use it directly!
        if (root && SafeIsReadable(reinterpret_cast<void*>(root + 0x3bc), 4)) {
            uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(root + 0x3bc);
            if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                int count = *reinterpret_cast<int*>(selMgr + 0x28);
                uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                if (count > 0 && list && SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                    uintptr_t selUnit = *reinterpret_cast<uintptr_t*>(list);
                    if (selUnit) {
                        uint32_t selHandle = GetUnitHandle(game, selUnit);
                        if (selHandle && selHandle != hero && selHandle != excludeUnit) {
                            uint32_t tid = fnGetUnitTypeId(selHandle);
                            bool match = (targetTypeId != 0) ? (tid == targetTypeId)
                                                             : (tid == 0x68303430 || tid == 0x6E303035 || tid == 0x6E303138);
                            if (match) {
                                float sx = 0.0f, sy = 0.0f;
                                if (!hero || (coordsOk && GetUnitCoords(game, selHandle, sx, sy) &&
                                    ((sx - hx) * (sx - hx) + (sy - hy) * (sy - hy) <= radius * radius))) {
                                    return selHandle;
                                }
                            }
                        }
                    }
                }
            }
        }

        uint32_t grp = fnCreateGroup();
        if (!grp) return 0;

        // Method 0.5: If looking for Altar ('n018', Shop 2), it is owned by the local player!
        if (targetTypeId == 0x6E303138 && fnEnumPlayer && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnEnumPlayer))) {
            auto fnLocal = reinterpret_cast<uint32_t(__cdecl*)()>(jassLookup("GetLocalPlayer"));
            uint32_t lp = fnLocal ? fnLocal() : 0;
            if (lp) {
                fnEnumPlayer(grp, lp, 0);
                while (uint32_t u = fnFirstOfGroup(grp)) {
                    fnGroupRemoveUnit(grp, u);
                    if (u == hero || u == excludeUnit) continue;
                    if (fnGetUnitTypeId(u) == 0x6E303138) {
                        float ux = 0.0f, uy = 0.0f;
                        if (!hero || (coordsOk && GetUnitCoords(game, u, ux, uy) &&
                            ((ux - hx) * (ux - hx) + (uy - hy) * (uy - hy) <= radius * radius))) {
                            fnDestroyGroup(grp);
                            return u;
                        }
                    }
                }
            }
        }

        float bestDistSq = radius * radius;
        uint32_t bestShop = 0;

        // Method 1: Scan within radius around Hero (3500.0f covers entire MTH base)
        if (coordsOk && fnGroupEnumUnitsInRange &&
            SafeIsExecutable(reinterpret_cast<uintptr_t>(fnGroupEnumUnitsInRange))) {
            __try {
                fnGroupEnumUnitsInRange(grp, &hx, &hy, &radius, 0);
                while (uint32_t u = fnFirstOfGroup(grp)) {
                    fnGroupRemoveUnit(grp, u);
                    if (u == hero || u == excludeUnit || (activeCourierHandle && u == activeCourierHandle)) continue;

                    uint32_t tid = fnGetUnitTypeId(u);
                    if (tid == 0x6E776774) continue; // Skip Way Gate ('nwgt')

                    bool match = (targetTypeId != 0) ? (tid == targetTypeId)
                                                     : (tid == 0x68303430 || tid == 0x6E303035 || tid == 0x6E303138);
                    if (match) {
                        float ux = 0.0f, uy = 0.0f;
                        if (GetUnitCoords(game, u, ux, uy)) {
                            float dx = ux - hx, dy = uy - hy;
                            float dsq = dx * dx + dy * dy;
                            if (dsq <= bestDistSq) {
                                bestDistSq = dsq;
                                bestShop = u;
                            }
                        } else if (!bestShop) {
                            bestShop = u;
                        }
                    }
                }
            } __except (1) {}
        }

        uint32_t foundShop = bestShop;

        // Method 2 Fallback: Scan players (neutrals and player bases) if range scan found nothing
        if (!foundShop && fnPlayer && fnEnumPlayer &&
            SafeIsExecutable(reinterpret_cast<uintptr_t>(fnPlayer)) &&
            SafeIsExecutable(reinterpret_cast<uintptr_t>(fnEnumPlayer))) {
            __try {
                int pIds[] = { 15, 12, 11, 13, 14, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
                for (int pId : pIds) {
                    uint32_t p = fnPlayer(pId);
                    if (!p) continue;
                    fnEnumPlayer(grp, p, 0);
                    while (uint32_t u = fnFirstOfGroup(grp)) {
                        fnGroupRemoveUnit(grp, u);
                        if (u == hero || u == excludeUnit || (activeCourierHandle && u == activeCourierHandle)) continue;

                        uint32_t tid = fnGetUnitTypeId(u);
                        if (tid == 0x6E776774) continue;

                        bool match = (targetTypeId != 0) ? (tid == targetTypeId)
                                                         : (tid == 0x68303430 || tid == 0x6E303035 || tid == 0x6E303138);
                        if (match) {
                            if (coordsOk) {
                                float ux = 0.0f, uy = 0.0f;
                                if (GetUnitCoords(game, u, ux, uy)) {
                                    float dx = ux - hx, dy = uy - hy;
                                    float dsq = dx * dx + dy * dy;
                                    if (dsq <= bestDistSq) {
                                        bestDistSq = dsq;
                                        bestShop = u;
                                    }
                                } else if (!bestShop) {
                                    bestShop = u;
                                }
                            } else if (!bestShop) {
                                bestShop = u;
                            }
                        }
                    }
                }
                foundShop = bestShop;
            } __except (1) {}
        }

        fnDestroyGroup(grp);
        return foundShop;
    }

    void ReportExactRecipe(MthResult result) {
        if (!mthNoti || result == MthResult::Running || result == MthResult::Idle) return;
        const char* message = "Da dung chuoi lenh mua/ghep; khong tu thu lai";
        switch (result) {
        case MthResult::Submitted: message = "Da gui du chuoi lenh mua/ghep; game xu ly ket qua"; break;
        case MthResult::Disabled: message = "Can bat MTH49/G17, dong chat va dua game len truoc"; break;
        case MthResult::Busy: message = "Dang co chuoi mua/ghep khac"; break;
        case MthResult::NoHero: message = "Hay chon Hero con song cua ban"; break;
        case MthResult::Gold: message = "Khong du gold: Frozen 30000, Phoenix 75000 (45000 neu co I02E)"; break;
        case MthResult::NoShop: message = "Khong tim thay shop can thiet trong 777 quanh Hero"; break;
        case MthResult::Changed: message = "Da dung vi doi lua chon, Hero hoac tran dau"; break;
        case MthResult::Unavailable: message = "Shop chua co du nut mua/ghep; da dung"; break;
        default: break;
        }
        feedbackQueue.push_back(std::string("|cfffEBA0E[MTH]|r ") + message);
    }

    bool ExecuteExactRecipe(uintptr_t game, uintptr_t root, void* (*jassLookup)(const char*),
                            bool (*clickDirectFn)(int), int kind) {
        if (mthComboActive || pendingComboType >= 0) { ReportExactRecipe(MthResult::Busy); return false; }
        if (!game || !root || !jassLookup || !clickDirectFn) { ReportExactRecipe(MthResult::NoHero); return false; }
        MthRecipeNative api(game, root, jassLookup, clickDirectFn);
        const auto result = StartMthRecipe(exactRecipe, api, kind,
            exactComboAllowed && (mth49Enabled || g17Enabled), GetTickCount());
        mthComboActive = exactRecipe.Active();
        if (mthComboActive) {
            mthClickDirectFn = clickDirectFn;
            mthComboStep = MTH_COMBO_IDLE; // Never feed these stages into the legacy two-shop loop.
        }
        ReportExactRecipe(result);
        return result == MthResult::Running;
    }

    bool ExecuteMthCombo(uintptr_t game, uintptr_t root, void* (*jassLookup)(const char*),
                         bool (*clickDirectFn)(int), bool (*clickButtonFn)(bool, int, bool),
                         int comboType) { // 0=Shield, 1=Ice, 2=Wild, 3=Frozen, 4=Phoenix
        (void)clickButtonFn;
        if (comboType == 3 || comboType == 4) return ExecuteExactRecipe(game, root, jassLookup, clickDirectFn, comboType);
        if (!game || !jassLookup) return false;
        if (!mth49Enabled && !g17Enabled) {
            mth49Enabled = true; // Auto enable on command
        }
        if (mthComboActive) {
            return false;
        }

        uint32_t hero = FindLocalHero(game, jassLookup, root);
        if (!hero) {
            if (mthNoti) feedbackQueue.push_back("|cfffEBA0E[MTH]|r Khong tim thay Hero cua ban");
            return false;
        }

        int goldCost = 0;
        uint32_t shopRawcode1 = 0;
        uint32_t shopRawcode2 = 0;
        std::vector<int> slots1;
        std::vector<int> slots2;
        std::string skillName;

        struct ComboSlotDef {
            int slot;
            std::vector<uint32_t> rawcodes; // Chap nhan tat ca rawcode tuong thich (MTH custom & Blizzard standard)
            const char* name;
        };
        std::vector<ComboSlotDef> slotDefs1;

        const uint32_t SHOP_SHIELD  = 0x6E303035; // 'n005'
        const uint32_t SHOP_SKILLS  = 0x68303430; // 'h040'
        const uint32_t SHOP_COMBINE = 0x6E303138; // 'n018'

        if (comboType == 0) { // Shield = Rune Frost + Crystalball + PhilosophersStone + ArcaniteAmor
            goldCost = 10000;
            shopRawcode1 = SHOP_SHIELD;
            slotDefs1 = {
                {
                    0,
                    {
                        0x49303244, /* 'I02D' MTH Giap sat / Arcanite Armor */
                        0x61726172  /* 'arar' Blizzard Arcanite Armor */
                    },
                    "ArcaniteAmor"
                },
                {
                    3,
                    {
                        0x49303151, /* 'I01Q' MTH Khien go / Rune Frost */
                        0x7266726F, /* 'rfro' Blizzard Frost Rune / Rune of Frost */
                        0x726F6672, /* 'rofr' Rune of Frost alias */
                        0x726E6672, /* 'rnfr' Rune of Frost alias */
                        0x66727374  /* 'frst' Frost Item alias */
                    },
                    "Rune Frost"
                },
                {
                    6,
                    {
                        0x49303245, /* 'I02E' MTH Ngoc phong thu / Crystalball & PhilosophersStone */
                        0x63727362, /* 'crsb' Blizzard Crystal Ball */
                        0x63727973, /* 'crys' Crystal Ball alias */
                        0x70687374  /* 'phst' Blizzard Philosopher's Stone */
                    },
                    "Crystalball / PhilosophersStone"
                }
            };
            shopRawcode2 = SHOP_COMBINE;
            slots2 = { 1 };
            skillName = "Shield";
        } else if (comboType == 1) { // Ice
            goldCost = 10000;
            shopRawcode1 = SHOP_SKILLS;
            slots1 = { 0, 3, 6, 9 };
            skillName = "Ice";
        } else if (comboType == 2) { // Wild
            goldCost = 10000;
            shopRawcode1 = SHOP_SKILLS;
            slots1 = { 1, 4, 7, 10 };
            skillName = "Wild";
        }

        // 1. Quet kho do Hero: Kiem tra cac mon da co trong balo de khong mua lai
        auto resolveJass = [&](const char* n) { void* p = jassLookup ? jassLookup(n) : nullptr; return p ? p : LookupJassNative(game, n); };
        auto fnUnitItemInSlot = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(resolveJass("UnitItemInSlot"));
        auto fnGetItemTypeId = reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(resolveJass("GetItemTypeId"));

        std::vector<uint32_t> heroBagItems;
        if (fnUnitItemInSlot && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnUnitItemInSlot))) {
            for (int s = 0; s < 6; ++s) {
                uint32_t itm = fnUnitItemInSlot(hero, s);
                if (itm && fnGetItemTypeId && SafeIsExecutable(reinterpret_cast<uintptr_t>(fnGetItemTypeId))) {
                    uint32_t tid = fnGetItemTypeId(itm);
                    if (tid) heroBagItems.push_back(tid);
                }
            }
        }

        int originalSlotCount = 0;
        int alreadyOwnedCount = 0;
        if (!slotDefs1.empty()) {
            originalSlotCount = static_cast<int>(slotDefs1.size());
            std::vector<uint32_t> tempBag = heroBagItems;
            for (const auto& def : slotDefs1) {
                bool found = false;
                for (uint32_t code : def.rawcodes) {
                    auto it = std::find(tempBag.begin(), tempBag.end(), code);
                    if (it != tempBag.end()) {
                        // Mon nay da co trong balo Hero (Rune Frost, Crystalball, PhilosophersStone hoac ArcaniteAmor) -> BO QUA khong mua lai!
                        alreadyOwnedCount++;
                        tempBag.erase(it);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    // Mon nay chua co -> them vao danh sach can mua
                    slots1.push_back(def.slot);
                }
            }
        } else {
            originalSlotCount = static_cast<int>(slots1.size());
        }

        // 2. Tinh toan luong gold thuc te can (chi tinh tien nhung mon chua co)
        int actualGoldCost = goldCost;
        if (originalSlotCount > 0) {
            if (slots1.empty()) {
                actualGoldCost = 0; // Da co du nguyen lieu, khong ton gold
            } else if (static_cast<int>(slots1.size()) < originalSlotCount) {
                actualGoldCost = (goldCost * static_cast<int>(slots1.size())) / originalSlotCount;
            }
        }

        // Check player gold
        auto fnGetLocalPlayer = reinterpret_cast<uint32_t(__cdecl*)()>(resolveJass("GetLocalPlayer"));
        auto fnGetPlayerState = reinterpret_cast<int(__cdecl*)(uint32_t, int)>(resolveJass("GetPlayerState"));

        uint32_t localPlayer = fnGetLocalPlayer ? fnGetLocalPlayer() : 0;
        if (localPlayer && fnGetPlayerState && actualGoldCost > 0) {
            int gold = fnGetPlayerState(localPlayer, 1); // PLAYER_STATE_RESOURCE_GOLD = 1
            if (gold < actualGoldCost) {
                if (mthNoti) feedbackQueue.push_back("|cfffEBA0E[MTH]|r Khong du gold (can " + std::to_string(actualGoldCost) + " gold)");
                return false;
            }
        }

        // Frozen/Phoenix no longer reach this legacy worker; see mth_recipe.hpp.

        auto fnClearSelection = reinterpret_cast<void(__cdecl*)()>(resolveJass("ClearSelection"));
        auto fnSelectUnit = reinterpret_cast<void(__cdecl*)(uint32_t, uint32_t)>(resolveJass("SelectUnit"));

        // Lay unit dang duoc nguoi choi chon de bao toan sau khi mua & ep (thuong la Hero)
        uint32_t origSel = hero;
        if (root && SafeIsReadable(reinterpret_cast<void*>(root + 0x3bc), 4)) {
            uintptr_t selMgr = *reinterpret_cast<uintptr_t*>(root + 0x3bc);
            if (selMgr && selMgr != static_cast<uintptr_t>(-0x10) && SafeIsReadable(reinterpret_cast<void*>(selMgr + 0x28), 8)) {
                int count = *reinterpret_cast<int*>(selMgr + 0x28);
                uintptr_t list = *reinterpret_cast<uintptr_t*>(selMgr + 0x30);
                if (count > 0 && list && SafeIsReadable(reinterpret_cast<void*>(list), 4)) {
                    uintptr_t selUnit = *reinterpret_cast<uintptr_t*>(list);
                    if (selUnit) {
                        uint32_t h = GetUnitHandle(game, selUnit);
                        if (IsValidJassHandle(h)) origSel = h;
                    }
                }
            }
        }

        // 3. Truong hop Hero DA CO DU TAT CA NGUYEN LIEU (khong can mua them mon nao tu Shop 1)
        if (slots1.empty()) {
            pendingComboType = -1;
            if (shopRawcode2 && !slots2.empty()) {
                uint32_t shop2 = FindShopUnit(game, hero, shopRawcode2, jassLookup, 950.0f, root);
                if (!shop2) {
                    shop2 = FindShopUnit(game, hero, shopRawcode2, jassLookup, 3500.0f, root);
                }
                if (shop2 && clickDirectFn && fnClearSelection && fnSelectUnit) {
                    mthComboHero = origSel;
                    mthComboShop1 = 0;
                    mthComboShop2 = shop2;
                    mthComboSlots1.clear();
                    mthComboSlots2 = slots2;
                    mthComboAlreadyOwned = alreadyOwnedCount;
                    mthClickDirectFn = clickDirectFn;
                    mthComboSkillName = skillName;
                    mthComboActive = true;
                    mthComboStep = MTH_COMBO_WAIT_SHOP_2;
                    mthComboNextTick = GetTickCount() + 100;
                    fnClearSelection();
                    fnSelectUnit(shop2, 1);
                    if (mthNoti) {
                        feedbackQueue.push_back("|cfffEBA0E[MTH]|r Da co du nguyen lieu, tu dong ep " + skillName + "!");
                    }
                    return true;
                }
            }
            if (mthNoti) {
                feedbackQueue.push_back("|cfffEBA0E[MTH]|r Ban da co day du " + skillName + "!");
            }
            return true;
        }

        // 4. Truong hop can mua cac mon con thieu: Tim Shop 1 trong ban kinh 950 quanh Hero
        uint32_t shop1 = FindShopUnit(game, hero, shopRawcode1, jassLookup, 950.0f, root);
        if (!shop1 && shopRawcode1 == SHOP_SHIELD) {
            shop1 = FindShopUnit(game, hero, SHOP_SKILLS, jassLookup, 950.0f, root);
        }
        if (!shop1) {
            // Hero chua o gan Shop 1 (khoang cach > 950) -> luu pending de khi den gan se tu mua & ep
            if (mthNoti && pendingComboType != comboType) {
                std::string msg = "|cfffEBA0E[MTH]|r Cho Hero den gan Shop de ";
                if (alreadyOwnedCount > 0) {
                    msg += "mua " + std::to_string(slots1.size()) + " mon con thieu & ep " + skillName + "!";
                } else {
                    msg += "tu dong mua & ep " + skillName + "!";
                }
                feedbackQueue.push_back(msg);
            }
            pendingComboType = comboType;
            mthClickDirectFn = clickDirectFn;
            return true;
        }

        pendingComboType = -1; // Da o gan shop, thuc thi ngay

        // Tim shop 2 (Nha Ghep 'n018')
        uint32_t shop2 = 0;
        if (shopRawcode2) {
            shop2 = FindShopUnit(game, hero, shopRawcode2, jassLookup, 950.0f, root, shop1);
            if (!shop2) {
                shop2 = FindShopUnit(game, hero, shopRawcode2, jassLookup, 3500.0f, root, shop1);
            }
        }

        // Khoi dong chuoi mua & ep do tu dong (chi mua cac slot trong slots1 - tuc nhung mon chua co)
        mthComboHero = origSel;
        mthComboShop1 = shop1;
        mthComboShop2 = shop2;
        mthComboSlots1 = slots1;
        mthComboSlots2 = slots2;
        mthComboAlreadyOwned = alreadyOwnedCount;
        mthClickDirectFn = clickDirectFn;
        mthComboSkillName = skillName;
        mthComboActive = true;
        mthComboStep = MTH_COMBO_WAIT_SHOP_1;
        mthComboNextTick = GetTickCount() + 100;

        // Chon Shop 1 de engine Warcraft 3 nap cac nut bam vao CCommandBar
        if (fnClearSelection && fnSelectUnit) {
            fnClearSelection();
            fnSelectUnit(shop1, 1);
        }

        return true;
    }
};

} // namespace wht
