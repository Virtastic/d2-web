// 1.13c D2Game+0xEAD10: item stat adjust by the class row at dt[0xBC4] + class*0xC4 (callers: 0xEAF80). EAX = count, ESI = unit.
// Stat thunks, all plain stdcall with the unit first: 0xA4D2 set(unit, stat, value, 0), 0xA42A get(unit, stat, 0), 0xA472 clamp(unit, stat, value, 0).
#include <cstdint>
#include <cstring>
struct GEAD10Env {
    uint32_t (*statAdd)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer);   // 0xA4D2
    int32_t  (*statGet)(uint32_t unit, uint32_t stat, uint32_t layer);                   // 0xA42A
    uint32_t (*statSet)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer);   // 0xA472
};
// 1.13c D2Game+0xEAD10
void g_ead10(int32_t count, uint32_t unit, uint32_t dt, const GEAD10Env& env)
{
    auto rd = [](uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; };
    if (count == 0 || unit == 0) return;
    int32_t cls = (int32_t)rd(unit, 4);
    if (cls < 0 || cls >= (int32_t)rd(dt, 0xbc8)) return;
    uint32_t row = rd(dt, 0xbc4) + (uint32_t)cls * 0xc4;
    if (row == 0) return;
    const uint32_t r46 = ((const uint8_t*)(uintptr_t)row)[0x46], r47 = ((const uint8_t*)(uintptr_t)row)[0x47];
    const uint32_t c = (uint32_t)count;
    env.statAdd(unit, 4, 0u - c, 0);
    env.statAdd(unit, 3, c, 0);
    env.statAdd(unit, 7, (r46 * c) << 6, 0);
    if (count > 0) env.statAdd(unit, 6, (r46 * c) << 6, 0);
    int32_t hi = env.statGet(unit, 7, 0);
    if (env.statGet(unit, 6, 0) > hi) env.statSet(unit, 6, (uint32_t)hi, 0);
    env.statAdd(unit, 0xb, (r47 * c) << 6, 0);
    if (count > 0) env.statAdd(unit, 0xa, (r47 * c) << 6, 0);
    hi = env.statGet(unit, 0xb, 0);
    if (env.statGet(unit, 0xa, 0) > hi) env.statSet(unit, 0xa, (uint32_t)hi, 0);
}
