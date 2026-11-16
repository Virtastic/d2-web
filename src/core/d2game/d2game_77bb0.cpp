// 1.13c D2Game+0x77BB0: one step of a unit's life drain (caller 0x79B90). EDI = pUnit, no stack
// args; EAX = 1 while the unit lives on. Callees are D2Common thunks (all stdcall): 0xA478 GetRoom, 0xA808 room test,
// 0xA7AE inventory item at a body location, 0xA53E item record, 0xA802 stat (unit, stat, layer), 0xA4D2 AddUnitStat,
// 0xA42A GetUnitStat, 0xA472 SetUnitStat.
#include <cstdint>
struct G77BB0Env {
    uint32_t (*room)(uint32_t unit);                                                 // 0xA478
    uint32_t (*roomTest)(uint32_t room);                                             // 0xA808
    uint32_t (*invItem)(uint32_t inv, uint32_t loc);                                 // 0xA7AE
    uint32_t (*itemRec)(uint32_t id);                                                // 0xA53E
    int32_t  (*stat)(uint32_t unit, uint32_t stat, uint32_t layer);                  // 0xA802
    void     (*addStat)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer); // 0xA4D2
    int32_t  (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);               // 0xA42A
    void     (*setStat)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer); // 0xA472
};
static uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
// 1.13c D2Game+0x77BB0
uint32_t g_77bb0(uint32_t unit, uint32_t dt, const G77BB0Env& env)
{
    if (unit == 0) return 0;
    if (env.roomTest(env.room(unit)) != 0) return 1;
    const int32_t cls = (int32_t)rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)rd32(dt + 0xBC8)) return 0;
    const uint32_t rec = (uint32_t)cls * 0xC4 + rd32(dt + 0xBC4);
    if (rec == 0) return 0;
    int32_t n = (int32_t)(*(const uint8_t*)(uintptr_t)(rec + 0x42)) * 2;
    const uint32_t inv = rd32(unit + 0x60);
    if (inv == 0) return 0;
    const uint32_t it = env.invItem(inv, 3);
    if (it != 0) {
        const uint32_t r = env.itemRec(rd32(it + 4));
        if (r != 0) n = (int32_t)((uint32_t)n * (uint32_t)((int32_t)rd32(r + 0xD8) / 10 + 1));
    }
    const int32_t v = env.stat(unit, 0x9A, 0);
    if (v != 0) n += (int32_t)((uint32_t)v * (uint32_t)n) / -100;   // retail: signed division by -100 (magic 0xAE147AE1)
    if (n < 1) n = 1;
    env.addStat(unit, 10, 0u - (uint32_t)n, 0);
    if (env.getStat(unit, 10, 0) > 0) return 1;
    env.setStat(unit, 10, 0, 0);
    return 0;
}
