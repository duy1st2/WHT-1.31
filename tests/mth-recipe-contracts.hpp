inline void MthCheck129(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
namespace mth_recipe_fixture {
struct Api {
    wht::MthHero hero{0x1000, 0x2000, 1, 2, 10, 100000, {}};
    uint32_t selected = 10;
    bool valid = true, capture = true, ready = true, shopValid = true;
    bool selectOk = true;
    uint32_t missingShop = 0;
    int failClick = -1;
    unsigned captures = 0;
    std::vector<uint32_t> selections;
    std::vector<std::pair<uint32_t, int>> clicks;
    bool Capture(wht::MthHero& h) { ++captures; h = hero; return capture; }
    bool Valid(const wht::MthHero&) { return valid; }
    uint32_t Selected() { return selected; }
    uint32_t FindShop(const wht::MthHero&, uint32_t code) { return code == missingShop ? 0 : code; }
    bool ShopValid(const wht::MthHero&, uint32_t, uint32_t) { return shopValid; }
    bool Select(uint32_t h) { if (!selectOk) return false; selected = h; selections.push_back(h); return true; }
    bool Ready(uint32_t, const wht::MthStage&) { return ready; }
    bool Click(int i) { if (i == failClick) return false; clicks.emplace_back(selected, i); return true; }
};
void Slots(const wht::MthStage& stage, uint32_t shop, std::initializer_list<int> slots) {
    MthCheck129(stage.shop == shop && stage.count == slots.size() &&
        std::equal(slots.begin(), slots.end(), stage.slots.begin()), "ETS recipe shop and exact memory-slot order");
}
}
void TestMthRecipeContracts() {
    using namespace mth_recipe_fixture;
    using R = wht::MthResult;
    const uint32_t skills = 0x68303430, combine = 0x6e303138, shield = 0x6e303035;
    std::array<uint32_t, 6> bag{};
    auto recipe = wht::MakeMthRecipe(3, bag);
    MthCheck129(recipe.count == 2 && recipe.gold == 30000, "Frozen ETS 30000 gate and two phases");
    Slots(recipe.stages[0], skills, {0, 3, 6, 9, 1, 4, 7, 10}); Slots(recipe.stages[1], combine, {6});
    bag[0] = 0x49303244; recipe = wht::MakeMthRecipe(3, bag);
    Slots(recipe.stages[0], skills, {1, 4, 7, 10});
    bag[0] = 0x49303151; recipe = wht::MakeMthRecipe(3, bag);
    Slots(recipe.stages[0], skills, {0, 3, 6, 9});
    bag[1] = 0x49303244; recipe = wht::MakeMthRecipe(3, bag);
    Slots(recipe.stages[0], skills, {}); MthCheck129(recipe.gold == 30000, "Frozen owned components do not change ETS gold gate");
    bag.fill(0); recipe = wht::MakeMthRecipe(4, bag);
    MthCheck129(recipe.count == 4 && recipe.gold == 75000, "Phoenix ETS four phases and 75000 gate");
    Slots(recipe.stages[0], skills, {0, 3, 6, 9, 1, 4, 7, 10, 2, 5, 8});
    Slots(recipe.stages[1], combine, {6, 3, 0}); Slots(recipe.stages[2], shield, {6, 9, 1});
    Slots(recipe.stages[3], combine, {9, 10});
    bag[5] = 0x49303245; recipe = wht::MakeMthRecipe(4, bag);
    MthCheck129(recipe.gold == 45000 && recipe.count == 4, "Phoenix I02E gate is 45000");
    Slots(recipe.stages[0], skills, {2, 5, 8}); Slots(recipe.stages[1], combine, {3, 0});
    Slots(recipe.stages[2], shield, {6, 9, 1}); Slots(recipe.stages[3], combine, {9, 10});
    bag.fill(0); recipe = wht::MakeMthRecipe(4, bag);
    MthCheck129(recipe.gold == 75000 && recipe.stages[0].count == 11, "owned flags never leak across activations");
    Slots(wht::MakeMthRecipe(0,bag).stages[0],shield,{0,3,6}); Slots(wht::MakeMthRecipe(0,bag).stages[1],combine,{1}); Slots(wht::MakeMthRecipe(1,bag).stages[0],skills,{0,3,6,9}); Slots(wht::MakeMthRecipe(2,bag).stages[0],skills,{1,4,7,10}); MthCheck129(!wht::MakeMthRecipe(5,bag).count,"invalid recipe rejected");

    wht::MthRecipeRun run; Api api;
    MthCheck129(run.Start(4, 1000, true, api) == R::Running && api.selected == skills && api.clicks.empty(), "select first shop before buying");
    MthCheck129(run.Start(3, 1050, true, api) == R::Busy && api.captures == 1, "duplicate request cannot overwrite running recipe");
    MthCheck129(run.Tick(1099, true, api) == R::Running && api.clicks.empty(), "wait at least 100ms for card");
    MthCheck129(run.Tick(1100, true, api) == R::Running && api.clicks.size() == 11 && api.selected == combine, "Phoenix first burst then combine shop");
    run.Tick(1100, true, api); MthCheck129(api.clicks.size() == 11, "same-tick callback cannot replay purchases");
    run.Tick(1200, true, api); MthCheck129(api.clicks.size() == 14 && api.selected == shield, "Phoenix intermediate combines then n005");
    run.Tick(1300, true, api); MthCheck129(api.clicks.size() == 17 && api.selected == combine, "Phoenix n005 burst then n018 again");
    MthCheck129(run.Tick(1400, true, api) == R::Submitted && !run.Active() && api.selected == 10, "final burst restores captured hero");
    std::vector<std::pair<uint32_t, int>> expected;
    for (unsigned p = 0; p < recipe.count; ++p)
        for (unsigned i = 0; i < recipe.stages[p].count; ++i) expected.emplace_back(recipe.stages[p].shop, recipe.stages[p].slots[i]);
    MthCheck129(api.clicks == expected && api.selections == std::vector<uint32_t>({skills, combine, shield, combine, 10}), "full golden Phoenix sequence matches ETS callbacks");
    MthCheck129(run.Tick(2000, true, api) == R::Idle && api.clicks.size() == 19, "no completion replay");
    api = {}; api.hero.bag.fill(0x12345678); // Full bag, no heuristic capacity gate.
    MthCheck129(run.Start(3, 2000, true, api) == R::Running, "six occupied slots do not block ETS combo");
    run.Tick(2100, true, api); MthCheck129(run.Tick(2200, true, api) == R::Submitted && api.clicks.size() == 9, "Frozen full sequence");
    api = {}; api.hero.bag[0] = 0x49303245; api.hero.gold = 45000;
    MthCheck129(run.Start(4, 1000, true, api) == R::Running, "Phoenix with Frozen starts at exact lower threshold");
    for (uint32_t t : {1100u, 1200u, 1300u, 1400u}) run.Tick(t, true, api);
    MthCheck129(api.clicks.size() == 10, "Phoenix owned Frozen skips eight base clicks and first combine only");
    api = {}; api.hero.gold = 74999;
    MthCheck129(run.Start(4, 1000, true, api) == R::Gold && api.selections.empty(), "Phoenix without I02E needs 75000");
    api.hero.bag[0] = 0x49303245; api.hero.gold = 44999;
    MthCheck129(run.Start(4, 1000, true, api) == R::Gold, "Phoenix lower gold boundary");
    api.hero.gold = 29999; MthCheck129(run.Start(3, 1000, true, api) == R::Gold, "Frozen gold boundary");
    api = {}; MthCheck129(run.Start(3, 1000, false, api) == R::Disabled && api.captures == 0, "permission gate precedes native reads");
    api.capture = false; MthCheck129(run.Start(3, 1000, true, api) == R::NoHero && !run.Active(), "invalid selected hero blocks start");
    api = {}; api.missingShop = skills;
    MthCheck129(run.Start(3, 1000, true, api) == R::NoShop && !run.Active() && api.selected == 10, "missing first shop does not arm delayed purchase");
    api = {}; run.Start(4, 1000, true, api); api.missingShop = combine;
    MthCheck129(run.Tick(1100, true, api) == R::NoShop && !run.Active() && api.selected == 10, "missing later shop cancels and restores");
    api = {}; run.Start(3, 1000, true, api); api.selected = 999;
    MthCheck129(run.Tick(1100, true, api) == R::Changed && api.selected == 999 && api.clicks.empty(), "user selection wins; no wrong-card click or forced restore");
    api = {}; run.Start(3, 1000, true, api); api.valid = false;
    MthCheck129(run.Tick(1100, true, api) == R::Changed && api.clicks.empty() && api.selections.size() == 1, "dead/recycled hero or session change discards stale handles");
    api = {}; run.Start(3, 1000, true, api);
    MthCheck129(run.Tick(1050, false, api) == R::Disabled && api.selected == 10 && api.clicks.empty(), "chat/focus/permission cancellation before purchase");
    api = {}; run.Start(3, 1000, true, api); api.ready = false;
    MthCheck129(run.Tick(1100, true, api) == R::Running && api.clicks.empty(), "not-ready command card waits");
    MthCheck129(run.Tick(2500, true, api) == R::Unavailable && api.selected == 10 && !run.Active(), "card timeout cancels without retries");
    api = {}; run.Start(3, 1000, true, api); api.ready = false; run.Tick(1100, true, api); api.ready = true;
    MthCheck129(run.Tick(1200, true, api) == R::Running && api.clicks.size() == 8, "late card buys once when ready");
    run.Reset(); api = {}; run.Start(3, 1000, true, api); api.failClick = 3;
    MthCheck129(run.Tick(1100, true, api) == R::Failed && api.clicks.size() == 1 && api.selected == 10, "failed purchase aborts without replaying prior gold spend");
    api = {}; run.Start(3, 0xffffffc0u, true, api);
    MthCheck129(run.Tick(0x23u, true, api) == R::Running && api.clicks.empty(), "100ms wait survives tick wrap");
    MthCheck129(run.Tick(0x24u, true, api) == R::Running && api.clicks.size() == 8, "wrapped deadline fires once");
    run.Reset(); api = {}; run.Start(3, 1000, true, api); api.shopValid = false;
    MthCheck129(run.Tick(1100, true, api) == R::NoShop && api.clicks.empty(), "removed or out-of-range shop cancels");
    api = {}; api.selectOk = false;
    MthCheck129(run.Start(3, 1000, true, api) == R::Failed && !run.Active(), "failed selection cannot leave worker armed");
}
