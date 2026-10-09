#pragma once
#include <array>
#include <cstdint>
#include <initializer_list>

namespace wht {
// Exact ETS.mix command-card memory indices, NOT row-major UI indices.
// Evidence: tests/FROZEN-PHOENIX-EVIDENCE.md.
struct MthStage {
    uint32_t shop = 0;
    std::array<int, 11> slots{};
    unsigned count = 0;
    void Add(std::initializer_list<int> values) { for (int v : values) slots[count++] = v; }
};
struct MthRecipe {
    std::array<MthStage, 4> stages{};
    unsigned count = 0;
    int gold = 0;
};
inline MthRecipe MakeMthRecipe(int kind, const std::array<uint32_t, 6>& bag) {
    bool ice = false, wild = false, frozen = false;
    for (auto id : bag) {
        ice |= id == 0x49303244; // I02D
        wild |= id == 0x49303151; // I01Q
        frozen |= id == 0x49303245; // I02E
    }
    MthRecipe r;
    if(kind>=0&&kind<=2){
        r.count=kind==0?2:1;r.gold=10000;
        r.stages[0].shop=kind==0?0x6e303035:0x68303430;
        if(kind==0){
            bool armor=false,rune=false,crystal=false;
            for(auto id:bag){
                armor|=id==0x49303244||id==0x61726172;
                rune|=id==0x49303151||id==0x7266726f||id==0x726f6672||id==0x726e6672||id==0x66727374;
                crystal|=id==0x49303245||id==0x63727362||id==0x63727973||id==0x70687374;
            }
            if(!armor)r.stages[0].Add({0});if(!rune)r.stages[0].Add({3});if(!crystal)r.stages[0].Add({6});
            r.stages[1].shop=0x6e303138;r.stages[1].Add({1});
        }else if(kind==1)r.stages[0].Add({0,3,6,9});
        else r.stages[0].Add({1,4,7,10});
        return r;
    }
    if (kind != 3 && kind != 4) return r;
    r.stages[0].shop = 0x68303430; // h040
    r.stages[1].shop = 0x6e303138; // n018
    if (kind == 3) {
        r.count = 2; r.gold = 30000; // ETS retains this gate even with both components.
        if (!ice) r.stages[0].Add({0, 3, 6, 9});
        if (!wild) r.stages[0].Add({1, 4, 7, 10});
        r.stages[1].Add({6});
    } else {
        r.count = 4; r.gold = frozen ? 45000 : 75000;
        if (!frozen) r.stages[0].Add({0, 3, 6, 9, 1, 4, 7, 10});
        r.stages[0].Add({2, 5, 8});
        if (!frozen) r.stages[1].Add({6});
        r.stages[1].Add({3, 0});
        r.stages[2].shop = 0x6e303035; // n005
        r.stages[2].Add({6, 9, 1});
        r.stages[3].shop = 0x6e303138;
        r.stages[3].Add({9, 10});
    }
    return r;
}
struct MthHero {
    uintptr_t object = 0, session = 0;
    uint32_t low = 0, high = 0, handle = 0;
    int gold = 0;
    std::array<uint32_t, 6> bag{};
};
enum class MthResult { Idle, Running, Submitted, Disabled, Busy, NoHero, Gold, NoShop, Changed, Unavailable, Failed };

class MthRecipeRun {
    MthRecipe recipe_{};
    MthHero hero_{};
    uint32_t shop_ = 0, entered_ = 0;
    unsigned stage_ = 0;
    bool active_ = false;
    template<class Api> void Restore(Api& api) {
        // Never steal selection back if the user selected something else.
        if (shop_ && api.Valid(hero_) && api.Selected() == shop_) api.Select(hero_.handle);
    }
    template<class Api> MthResult SelectStage(uint32_t now, Api& api) {
        const auto next = api.FindShop(hero_, recipe_.stages[stage_].shop);
        if (!next) { Restore(api); Reset(); return MthResult::NoShop; }
        if (!api.Select(next)) { Restore(api); Reset(); return MthResult::Failed; }
        shop_ = next; entered_ = now;
        return MthResult::Running;
    }
public:
    bool Active() const { return active_; }
    void Reset() { *this = MthRecipeRun{}; }
    template<class Api> MthResult Start(int kind, uint32_t now, bool allowed, Api& api) {
        if (active_) return MthResult::Busy;
        Reset();
        if (!allowed) return MthResult::Disabled;
        if (!api.Capture(hero_)) return MthResult::NoHero;
        recipe_ = MakeMthRecipe(kind, hero_.bag);
        if (!recipe_.count) { Reset(); return MthResult::Failed; }
        if (hero_.gold < recipe_.gold) { Reset(); return MthResult::Gold; }
        active_ = true;
        return SelectStage(now, api);
    }
    template<class Api> MthResult Tick(uint32_t now, bool allowed, Api& api) {
        if (!active_) return MthResult::Idle;
        if (!api.Valid(hero_)) { Reset(); return MthResult::Changed; }
        if (!allowed) { Restore(api); Reset(); return MthResult::Disabled; }
        if (now - entered_ < 100) return MthResult::Running;
        if (api.Selected() != shop_) { Reset(); return MthResult::Changed; }
        const auto& stage = recipe_.stages[stage_];
        if (!api.ShopValid(hero_, shop_, stage.shop)) { Restore(api); Reset(); return MthResult::NoShop; }
        // Wait a bounded time for the selected shop's command card, without buying twice.
        if (!api.Ready(shop_, stage)) {
            if (now - entered_ < 1500) return MthResult::Running;
            Restore(api); Reset(); return MthResult::Unavailable;
        }
        for (unsigned i = 0; i < stage.count; ++i) {
            if (api.Selected() != shop_ || !api.Click(stage.slots[i])) {
                Restore(api); Reset(); return MthResult::Failed;
            }
        }
        if (++stage_ < recipe_.count) return SelectStage(now, api);
        Restore(api); Reset(); return MthResult::Submitted;
    }
};
} // namespace wht
