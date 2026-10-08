#pragma once
#include <windows.h>
#include <cstdint>
#include <cstring>
#include <cmath>

namespace wht {
inline constexpr uint32_t GokuItemType = 0x49303136; // I016, retained from the old MTH implementation.
// Game-thread only. Read natives never change the simulation; orders go through
// the engine's synchronized target-action builder (see AUTO-PICK-EVIDENCE.md).
class AutoPick {
    uintptr_t session_ = 0, unit_ = 0;
    uint32_t unitLow_ = 0, unitHigh_ = 0, targetLow_ = 0, targetHigh_ = 0;
    uint32_t scan_ = 0, submitted_ = 0;
    bool waiting_ = false;
    uint32_t itemType_ = 0; // Zero means normal Auto Pick (any item).
    bool completed_ = false;
    struct Failed { uint32_t low = 0, high = 0, tick = 0; bool used = false; } failed_[16]{};
    unsigned nextFailed_ = 0;
    static bool Readable(uintptr_t p, size_t size) {
        MEMORY_BASIC_INFORMATION m{};
        return p && VirtualQuery(reinterpret_cast<void*>(p), &m, sizeof(m)) == sizeof(m) &&
            m.State == MEM_COMMIT && !(m.Protect & (PAGE_GUARD | PAGE_NOACCESS)) &&
            p >= reinterpret_cast<uintptr_t>(m.BaseAddress) &&
            size <= m.RegionSize - (p - reinterpret_cast<uintptr_t>(m.BaseAddress));
    }
    static uintptr_t Ptr(uintptr_t p) { return Readable(p, 4) ? *reinterpret_cast<uintptr_t*>(p) : 0; }
    static uint32_t ShiftRva(unsigned rva) {
        if (ActiveGameOffsets().version == War3Version::v126a) {
            if (rva >= 0x2C0000 && rva <= 0x2D0000) return rva - 0xB20;
            if (rva >= 0x300000 && rva <= 0x440000) return rva - 0xB40;
            if (rva >= 0x440000 && rva <= 0x520000) return rva - 0xB10;
        }
        return rva;
    }
    static uint32_t Native(uintptr_t game, unsigned rva, uint32_t h) {
        return reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(game + ShiftRva(rva))(h);
    }
    static float Real(uintptr_t game, unsigned rva, uint32_t h) {
        const auto bits = Native(game, rva, h); float value; memcpy(&value, &bits, 4); return value;
    }
    static uint32_t Handle(uintptr_t game, uintptr_t context, uintptr_t object) {
        return reinterpret_cast<uint32_t(__thiscall*)(uintptr_t, uintptr_t, int)>(game + ActiveGameOffsets().lookupUnit)(context, object, 0);
    }
    bool Excluded(uint32_t low, uint32_t high, uint32_t now) const {
        for (const auto& f : failed_) if (f.used && f.low == low && f.high == high && now - f.tick < 10000) return true;
        return false;
    }
    void Fail(uint32_t now) {
        failed_[nextFailed_++ % 16] = {targetLow_, targetHigh_, now, true}; waiting_ = false;
    }
    void TickNative(uintptr_t game, uint32_t now) {
        const auto players = Ptr(game + ActiveGameOffsets().world);
        if (!Readable(players, 0x98)) { Reset(); return; }
        const auto slot = *reinterpret_cast<uint16_t*>(players + 0x28);
        if (slot >= 16) { Reset(); return; }
        const auto unit = Ptr(Ptr(Ptr(players + 0x58 + slot * 4) + 0x34) + 0x1E0);
        const auto tls = reinterpret_cast<uintptr_t(__fastcall*)(unsigned, unsigned)>(game + ActiveGameOffsets().eventContext)(13, 0);
        const auto session = Ptr(Ptr(tls + 0x10) + 8);
        if (!Readable(unit, 0x20) || !Readable(session, 0x2248) || Ptr(session + 0x610) ||
            *reinterpret_cast<int*>(session + 0x278) < 3) { Reset(); return; }
        const auto low = Ptr(unit + 0xC), high = Ptr(unit + 0x10);
        if (session_ != session || unit_ != unit || unitLow_ != low || unitHigh_ != high) {
            const auto filter = itemType_;
            Reset(); itemType_ = filter; session_ = session; unit_ = unit; unitLow_ = low; unitHigh_ = high;
        }
        const auto context = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(game + ActiveGameOffsets().getUnitManager)(players);
        if (!Readable(context, 4)) { Reset(); return; }
        const auto hero = Handle(game, context, unit);
        const auto local = reinterpret_cast<uint32_t(__cdecl*)()>(game + ActiveGameOffsets().localPlayer)();
        if (!hero || !local || Native(game, 0x3C8CD0, hero) != local ||
            !(Real(game, 0x3C51A0, hero) > 0.405f)) { Reset(); return; }
        const unsigned capacity = Native(game, 0x3C9A40, hero);
        if (!capacity || capacity > 6) { Reset(); return; }
        unsigned occupied = 0;
        for (unsigned i = 0; i < capacity; ++i) {
            const auto item = reinterpret_cast<uint32_t(__cdecl*)(uint32_t, int)>(game + ShiftRva(0x3C8270))(hero, i);
            const auto type = item ? Native(game, 0x3C57A0, item) : 0;
            if (type) ++occupied;
            // Completion is confirmed from inventory, never from submitting an order
            // or merely seeing the target disappear (someone else may pick it up).
            if (itemType_ && type == itemType_) { completed_ = true; waiting_ = false; return; }
        }
        if (occupied >= capacity) { waiting_ = false; return; }
        const float x = Real(game, 0x3C6050, hero), y = Real(game, 0x3C6090, hero);
        if (!std::isfinite(x) || !std::isfinite(y)) { Reset(); return; }
        const auto manager = Ptr(Ptr(game + ActiveGameOffsets().root) + 0x3BC);
        if (!Readable(manager + 0x614, 8)) { Reset(); return; }
        const auto count = Ptr(manager + 0x614), list = Ptr(manager + 0x618);
        if (count > 8192 || (count && !Readable(list, count * 4))) { Reset(); return; }
        uintptr_t best = 0; float distance = 777.0f * 777.0f; bool targetPresent = false;
        for (unsigned i = 0; i < count; ++i) {
            const auto item = Ptr(list + i * 4);
            if (!Readable(item, 0x34)) continue;
            const auto handle = Handle(game, context, item);
            // Handle type validation rejects deleted/non-item entries before getters.
            const auto type = handle ? Native(game, 0x3C57A0, handle) : 0;
            if (!type || (itemType_ && type != itemType_) || Native(game, 0x3C5AD0, handle) ||
                !Native(game, 0x3C5AB0, handle) || !(Real(game, 0x3C51A0, handle) > 0)) continue;
            const float ix = Real(game, 0x3C58D0, handle), iy = Real(game, 0x3C5910, handle);
            if (!std::isfinite(ix) || !std::isfinite(iy)) continue;
            if (!reinterpret_cast<uint32_t(__cdecl*)(const float*, const float*, uint32_t)>(game + ShiftRva(0x3CA430))(&ix, &iy, local)) continue;
            const auto itemLow = Ptr(item + 0xC), itemHigh = Ptr(item + 0x10);
            if (waiting_ && itemLow == targetLow_ && itemHigh == targetHigh_) targetPresent = true;
            if (Excluded(itemLow, itemHigh, now)) continue;
            const float dx = ix - x, dy = iy - y, d = dx * dx + dy * dy;
            if (d < distance) { distance = d; best = item; }
        }
        if (waiting_) {
            if (!targetPresent) waiting_ = false;
            else if (now - submitted_ >= 5000) { Fail(now); return; }
            else return; // No repeated orders while the unit walks to the item.
        }
        if (!best) return;
        // Match ETS's normal selected-unit smart order: no auxiliary agent.
        // ECX=order, EDX=0, stack=(target, flags), RET 8; builds action 0x12.
        reinterpret_cast<void(__fastcall*)(uint32_t, uintptr_t, uintptr_t, uint32_t)>(game + ShiftRva(0x2CC5F0))(0xD0003, 0, best, 0);
        targetLow_ = Ptr(best + 0xC); targetHigh_ = Ptr(best + 0x10);
        waiting_ = true; submitted_ = now;
    }
public:
    void Reset() { *this = AutoPick{}; }
    // Explicit clock also lets the real native loop be exercised by x86 fixtures.
    bool TickAt(uintptr_t game, bool enabled, uint32_t itemType, uint32_t now) {
        completed_ = false;
        if (!enabled || !game) { Reset(); return false; }
        if (itemType_ != itemType) { Reset(); itemType_ = itemType; }
        if (now - scan_ < 250) return false;
        __try { TickNative(game, now); }
        __except (EXCEPTION_EXECUTE_HANDLER) { Reset(); }
        scan_ = now;
        return completed_;
    }
    bool Tick(uintptr_t game, bool enabled, uint32_t itemType = 0) {
        return TickAt(game, enabled, itemType, GetTickCount());
    }
};
} // namespace wht
