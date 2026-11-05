// 1.13c D2Game+0xCA490 -- decrement the live counter of the table record a unit maps to, injectable callee.
// EAX = unit, stack word 0 = table (ret 4). Callee 0xA514 (D2Common #10830) takes the unit (ret 4); a zero result decrements rec[+0x2D0].
#include <cstdint>
struct GCA490Env { uint32_t (*ord10830)(uint32_t unit); };
static inline uint32_t g_r32(uint32_t p, uint32_t o) { return *(const uint32_t*)(uintptr_t)(p + o); }
// 1.13c D2Game+0xCA490
void g_ca490(uint32_t unit, uint32_t table, const GCA490Env& env)
{
    uint32_t idx = 0;
    if (unit && g_r32(unit, 0) == 1) {
        const uint32_t sub = g_r32(unit, 0x14);
        if (sub) {
            if (*(const uint8_t*)(uintptr_t)(sub + 0x5c) & 2) return;
            idx = g_r32(sub, 0x58);
        }
    }
    const uint32_t rec = g_r32(table, idx * 4);
    if (env.ord10830(unit) == 0) *(uint32_t*)(uintptr_t)(rec + 0x2d0) = g_r32(rec, 0x2d0) - 1;
}
