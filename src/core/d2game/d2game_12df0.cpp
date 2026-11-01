// Gold grant to a unit: the unit's gold stat (14) is raised by the amount up to its cap (0xA45A). When the
// unit is already at the cap the whole amount drops on the ground (0x12B90); when the amount overruns the cap
// the stat is filled to the cap and the remainder drops. The stat is set to 0 instead when the new value
// would be negative, or would exceed the cap of a player (unit type 0). Retail is 32-bit: the unit is in
// ESI, game and amount on the stack; the callees are seams in retail argument order.
#include <cstdint>
#include <cstring>

struct D2G12DF0Env {
    int32_t (*goldCap)(void* unit);                                        // #10049 0xA45A
    int32_t (*getStat)(void* unit, int32_t stat, int32_t layer);           // #10973 0xA42A
    void (*setStat)(void* unit, int32_t stat, int32_t value, int32_t layer); // #10887 0xA472
    void (*addStat)(void* unit, int32_t stat, int32_t value, int32_t layer); // #10551 0xA4D2
    void (*dropGold)(void* game, void* unit, uint32_t amount);             // 0x12B90
};

namespace {
inline uint32_t rd32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
void grant(void* unit, int32_t add, const D2G12DF0Env* env) {
    const int32_t v = env->getStat(unit, 14, 0) + add;
    if (v < 0 || (rd32(unit) == 0 && v > env->goldCap(unit))) env->setStat(unit, 14, 0, 0);
    else env->addStat(unit, 14, add, 0);
}
}

// 1.13c D2Game+0x12DF0
extern "C" void d2g_12df0(void* unit, void* game, uint32_t amount, const D2G12DF0Env* env) {
    const uint32_t cap = (uint32_t)env->goldCap(unit);
    const uint32_t cur = (uint32_t)env->getStat(unit, 14, 0);
    if (cur == cap) { env->dropGold(game, unit, amount); return; }
    if (cur + amount > cap) {
        if (unit) grant(unit, (int32_t)(cap - cur), env);
        env->dropGold(game, unit, cur - cap + amount);
        return;
    }
    if (unit) grant(unit, (int32_t)amount, env);
}
