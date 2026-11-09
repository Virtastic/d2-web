// 1.13c D2Game+0xEAE20: item stat adjust by the class row at dt[0xBC4] + class*0xC4 (callers: 0xEAF80). EAX = count, ESI = unit.
// Stat thunks as in d2game_ead10.cpp (0xA4D2 add, 0xA42A get, 0xA472 set).
#include <cstdint>
#include <cstring>
struct GEAE20Env {
    uint32_t (*statAdd)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer);
    int32_t  (*statGet)(uint32_t unit, uint32_t stat, uint32_t layer);
    uint32_t (*statSet)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer);
};
// 1.13c D2Game+0xEAE20
void g_eae20(int32_t count, uint32_t unit, uint32_t dt, const GEAE20Env& env)
{
    auto rd = [](uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; };
    if (count == 0 || unit == 0) return;
    int32_t cls = (int32_t)rd(unit, 4);
    if (cls < 0 || cls >= (int32_t)rd(dt, 0xbc8)) return;
    uint32_t row = rd(dt, 0xbc4) + (uint32_t)cls * 0xc4;
    if (row == 0) return;
    const uint32_t r48 = ((const uint8_t*)(uintptr_t)row)[0x48];
    const uint32_t c = (uint32_t)count;
    env.statAdd(unit, 4, 0u - c, 0);
    env.statAdd(unit, 1, c, 0);
    if (count > 0) env.statAdd(unit, 8, (r48 * c) << 6, 0);
    env.statAdd(unit, 9, (r48 * c) << 6, 0);
    int32_t hi = env.statGet(unit, 9, 0);
    if (env.statGet(unit, 8, 0) > hi) env.statSet(unit, 8, (uint32_t)hi, 0);
}
