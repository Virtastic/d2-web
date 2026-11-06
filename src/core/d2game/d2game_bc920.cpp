// 1.13c D2Game+0xBC920 -- EBX = pGame. For each index below the data-table count (sgptDataTables+0xC4, re-read every pass) whose slot is active
// (0xA68E(game, i), 0xABEC(i)) and has a unit (0xA5D4(game, i)): 0xABB6(game, unit) then 0xABAA(unit). Returns 1 if any unit was notified, else 0.
#include <cstdint>
struct GBC920Env {
    int32_t (*count)();                                  // *(*(IAT 0xF829C)) + 0xC4
    uint32_t (*slotActive)(uint32_t game, uint32_t i);   // 0xA68E (stdcall 2)
    uint32_t (*slotFlag)(uint32_t i);                    // 0xABEC (stdcall 1)
    uint32_t (*slotUnit)(uint32_t game, uint32_t i);     // 0xA5D4 (stdcall 2)
    void (*attach)(uint32_t game, uint32_t unit);        // 0xABB6 (stdcall 2)
    void (*finish)(uint32_t unit);                       // 0xABAA (stdcall 1)
};
// 1.13c D2Game+0xBC920
uint32_t g_bc920(uint32_t game, const GBC920Env& env)
{
    uint32_t r = 0;
    for (int32_t i = 0; i < env.count(); ++i) {
        if (!env.slotActive(game, (uint32_t)i)) continue;
        if (!env.slotFlag((uint32_t)i)) continue;
        const uint32_t u = env.slotUnit(game, (uint32_t)i);
        if (!u) continue;
        env.attach(game, u);
        env.finish(u);
        r = 1;
    }
    return r;
}
