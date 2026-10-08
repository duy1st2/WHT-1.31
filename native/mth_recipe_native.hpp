#pragma once
#include <windows.h>
#include <cmath>
#include <cstring>
#include "mth_recipe.hpp"

namespace wht {
// Exact 1.24.4.6387 native adapter. Selection/UI clicks only; no item, gold,
// ability, or simulation writes. Called on the existing game-thread dispatcher.
class MthRecipeNative {
    using U0 = uint32_t(__cdecl*)();
    using U1 = uint32_t(__cdecl*)(uint32_t);
    using U2 = uint32_t(__cdecl*)(uint32_t, int);
    uintptr_t game_, root_;
    bool (*click_)(int);
    U0 local_, create_;
    U1 type_, owner_, life_, x_, y_, itemType_, first_;
    U2 isType_, itemSlot_;
    int(__cdecl* gold_)(uint32_t, int);
    void(__cdecl* clear_)();
    void(__cdecl* select_)(uint32_t, int);
    void(__cdecl* destroy_)(uint32_t);
    void(__cdecl* remove_)(uint32_t, uint32_t);
    void(__cdecl* enumerate_)(uint32_t, const float*, const float*, const float*, uint32_t);
    template<class F> static F Lookup(void* (*lookup)(const char*), const char* name) {
        auto p = lookup ? lookup(name) : nullptr;
        MEMORY_BASIC_INFORMATION m{};
        if (!p || !VirtualQuery(p, &m, sizeof(m)) || m.State != MEM_COMMIT ||
            (m.Protect & (PAGE_GUARD | PAGE_NOACCESS)) ||
            !(m.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))) return nullptr;
        return reinterpret_cast<F>(p);
    }
    static bool Readable(uintptr_t p, size_t n) {
        MEMORY_BASIC_INFORMATION m{};
        return p && VirtualQuery(reinterpret_cast<void*>(p), &m, sizeof(m)) && m.State == MEM_COMMIT &&
            !(m.Protect & (PAGE_NOACCESS | PAGE_GUARD)) &&
            n <= m.RegionSize - (p - reinterpret_cast<uintptr_t>(m.BaseAddress));
    }
    static uintptr_t Ptr(uintptr_t p) { return Readable(p, 4) ? *reinterpret_cast<uintptr_t*>(p) : 0; }
    static float Real(U1 fn, uint32_t h) { const auto bits = fn(h); float v; memcpy(&v, &bits, 4); return v; }
    bool Available() const {
        return game_ && root_ && click_ && local_ && create_ && type_ && owner_ && life_ && x_ && y_ &&
            itemType_ && first_ && isType_ && itemSlot_ && gold_ && clear_ && select_ && destroy_ && remove_ && enumerate_;
    }
    uintptr_t SelectedObject() const {
        const auto players = Ptr(game_ + ActiveGameOffsets().world);
        if (!Readable(players, 0x98)) return 0;
        const auto slot = *reinterpret_cast<uint16_t*>(players + 0x28);
        return slot < 16 ? Ptr(Ptr(Ptr(players + 0x58 + slot * 4) + 0x34) + 0x1e0) : 0;
    }
    uint32_t Handle(uintptr_t object) const {
        const auto players = Ptr(game_ + ActiveGameOffsets().world);
        if (!players || !Readable(object, 0x20)) return 0;
        const auto context = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(game_ + ActiveGameOffsets().getUnitManager)(players);
        if (!context) return 0;
        return reinterpret_cast<uint32_t(__thiscall*)(uintptr_t, uintptr_t, int)>(game_ + ActiveGameOffsets().lookupUnit)(context, object, 0);
    }
    uintptr_t Session() const {
        if (!Available() || Ptr(game_ + ActiveGameOffsets().root) != root_ || Ptr(game_ + ActiveGameOffsets().recipeRoot) != root_) return 0;
        const auto tls = reinterpret_cast<uintptr_t(__fastcall*)(unsigned, unsigned)>(game_ + ActiveGameOffsets().eventContext)(13, 0);
        const auto session = Ptr(Ptr(tls + 0x10) + 8);
        if (!Readable(session, 0x2248) || Ptr(session + 0x610) ||
            *reinterpret_cast<int*>(session + 0x278) < 3) return 0;
        return session;
    }
    bool LivingHero(uint32_t h) const {
        return h && isType_(h, 0) && !isType_(h, 1) && owner_(h) == local_() && Real(life_, h) > 0.405f;
    }
public:
    MthRecipeNative(uintptr_t game, uintptr_t root, void* (*lookup)(const char*), bool (*click)(int))
        : game_(game), root_(root), click_(click),
          local_(Lookup<U0>(lookup, "GetLocalPlayer")), create_(Lookup<U0>(lookup, "CreateGroup")),
          type_(Lookup<U1>(lookup, "GetUnitTypeId")), owner_(Lookup<U1>(lookup, "GetOwningPlayer")),
          life_(Lookup<U1>(lookup, "GetWidgetLife")), x_(Lookup<U1>(lookup, "GetUnitX")), y_(Lookup<U1>(lookup, "GetUnitY")),
          itemType_(Lookup<U1>(lookup, "GetItemTypeId")), first_(Lookup<U1>(lookup, "FirstOfGroup")),
          isType_(Lookup<U2>(lookup, "IsUnitType")), itemSlot_(Lookup<U2>(lookup, "UnitItemInSlot")),
          gold_(Lookup<decltype(gold_)>(lookup, "GetPlayerState")), clear_(Lookup<decltype(clear_)>(lookup, "ClearSelection")),
          select_(Lookup<decltype(select_)>(lookup, "SelectUnit")), destroy_(Lookup<decltype(destroy_)>(lookup, "DestroyGroup")),
          remove_(Lookup<decltype(remove_)>(lookup, "GroupRemoveUnit")), enumerate_(Lookup<decltype(enumerate_)>(lookup, "GroupEnumUnitsInRange")) {}
    uint32_t Selected() const { return Handle(SelectedObject()); }
    bool Capture(MthHero& h) const {
        h = {};
        h.session = Session(); if (!h.session) return false;
        h.object = SelectedObject(); h.handle = Handle(h.object);
        if (!LivingHero(h.handle)) return false;
        h.low = static_cast<uint32_t>(Ptr(h.object + 0xc)); h.high = static_cast<uint32_t>(Ptr(h.object + 0x10));
        h.gold = gold_(local_(), 1);
        for (int i = 0; i < 6; ++i) { const auto item = itemSlot_(h.handle, i); h.bag[i] = item ? itemType_(item) : 0; }
        return true;
    }
    bool Valid(const MthHero& h) const {
        return h.session && Session() == h.session && Readable(h.object, 0x20) &&
            Ptr(h.object + 0xc) == h.low && Ptr(h.object + 0x10) == h.high &&
            Handle(h.object) == h.handle && LivingHero(h.handle);
    }
    bool ShopValid(const MthHero& h, uint32_t shop, uint32_t rawcode) const {
        if (!shop || type_(shop) != rawcode || !(Real(life_, shop) > 0.405f)) return false;
        const float hx = Real(x_, h.handle), hy = Real(y_, h.handle), sx = Real(x_, shop), sy = Real(y_, shop);
        if (!std::isfinite(hx) || !std::isfinite(hy) || !std::isfinite(sx) || !std::isfinite(sy)) return false;
        const float dx = sx - hx, dy = sy - hy;
        return dx * dx + dy * dy <= 777.0f * 777.0f;
    }
    uint32_t FindShop(const MthHero& h, uint32_t rawcode) const {
        if (!Valid(h)) return 0;
        const float x = Real(x_, h.handle), y = Real(y_, h.handle), radius = 777.0f;
        if (!std::isfinite(x) || !std::isfinite(y)) return 0;
        const auto group = create_(); if (!group) return 0;
        uint32_t found = 0;
        __try {
            enumerate_(group, &x, &y, &radius, 0);
            // ETS selects the first matching nearby unit; no 3500-range fallback.
            for (unsigned n = 0; n < 8192; ++n) {
                const auto unit = first_(group); if (!unit) break;
                remove_(group, unit);
                if (ShopValid(h, unit, rawcode)) { found = unit; break; }
            }
        } __finally { destroy_(group); }
        return found;
    }
    bool Select(uint32_t unit) const {
        if (!Available() || !unit || !type_(unit)) return false;
        clear_(); select_(unit, 1); return true;
    }
    bool Ready(uint32_t shop, const MthStage& stage) const {
        if (Selected() != shop) return false;
        // Avoid clicking a stale card while the newly selected shop UI is loading.
        const auto panel = Ptr(root_ + 0x3f4);
        if (!panel || Ptr(panel + 0x238) != SelectedObject()) return false;
        const auto buttons = Ptr(Ptr(Ptr(Ptr(root_ + 0x3c8) + 0x154) + 8));
        if (!buttons) return false;
        for (unsigned i = 0; i < stage.count; ++i) {
            const auto button = buttons + stage.slots[i] * 0x1c0;
            const auto data = Ptr(button + 0x190);
            if ((Ptr(button) != game_ + 0x950d1c && Ptr(button) != game_ + 0x93ebc4) || !Readable(data, 0x10) || !Ptr(data + 8)) return false;
        }
        return true;
    }
    bool Click(int memoryIndex) const { return click_ && click_(memoryIndex); }
};
inline MthResult StartMthRecipe(MthRecipeRun& run, MthRecipeNative& api, int kind, bool allowed, uint32_t now) {
    __try { return run.Start(kind, now, allowed, api); }
    __except (EXCEPTION_EXECUTE_HANDLER) { run.Reset(); return MthResult::Failed; }
}
inline MthResult TickMthRecipe(MthRecipeRun& run, MthRecipeNative& api, bool allowed, uint32_t now) {
    __try { return run.Tick(now, allowed, api); }
    __except (EXCEPTION_EXECUTE_HANDLER) { run.Reset(); return MthResult::Failed; }
}
} // namespace wht
