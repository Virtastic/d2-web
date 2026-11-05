// 1.13c D2Game+0xDAE70, injectable callees. EDI = unit, stack word 0 = delta (ret 4). Raises the unit's stat 8
// by delta, capped at its maximum (ordinal thunk 0xA4EA, one word); returns the amount actually gained when
// the cap was reached, else delta; 0 for delta <= 0. 0xA42A reads (unit, 8, 0), 0xA472 writes (unit, 8, v, 0).
#include <cstdint>
struct GDAE70Env {
    int32_t (*a4ea)(uint32_t unit);
    int32_t (*a42a)(uint32_t unit, uint32_t stat, uint32_t layer);
    void    (*a472)(uint32_t unit, uint32_t stat, int32_t value, uint32_t layer);
};
// 1.13c D2Game+0xDAE70
int32_t g_dae70(uint32_t unit, int32_t delta, const GDAE70Env& env)
{
    if (delta <= 0) return 0;
    const int32_t mx = env.a4ea(unit);
    const int32_t cur = env.a42a(unit, 8, 0);
    int32_t t = (int32_t)((uint32_t)delta + (uint32_t)cur);
    if (t >= mx) t = mx;
    env.a472(unit, 8, t, 0);
    return t == mx ? (int32_t)((uint32_t)mx - (uint32_t)cur) : delta;
}
