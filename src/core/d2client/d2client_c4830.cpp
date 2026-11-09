// 1.13c D2Client+0xC4830 cltSkillStartCooldown: from a skill row (sgptDataTables+0xB98, stride 0x23C) read the dwords at +0x148, +0x14C, +0x150, +0x154 and,
// when the +0x150 value scaled to ticks (x1000 / 25) is non-zero, stamp the cooldown globals. Register argument from the disassembly: EDX = skill id (stack word unused, ret 4).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cC4830St { uint32_t stamp, a, b, c, d; };   // 0x10B9CC GetTickCount, 0x10B9D0, 0x10B9D4, 0x10B9D8, 0x11BEFC
static_assert(sizeof(D2cC4830St) == 20, "state");
static uint32_t c48_rd(const uint8_t* p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
// retail's imul-by-0x3E8 then signed divide by 25 (the 0x51EB851F idiom)
static int32_t c48_scale(uint32_t v) { return (int32_t)(v * 1000u) / 25; }
extern "C" {
// dt = *sgptDataTables; g = the five globals in the order above (0x10B9CC..0x10B9D8 are contiguous, 0x11BEFC last).
EMSCRIPTEN_KEEPALIVE uint32_t d2c_c4830(const uint8_t* dt, int32_t id, uint32_t tick, D2cC4830St* g) {
    uint32_t f148 = 0, f14c = 0, f150 = 0, f154 = 0;
    if (id >= 0 && id < (int32_t)c48_rd(dt, 0xba0)) {
        const uint8_t* rec = (const uint8_t*)(uintptr_t)c48_rd(dt, 0xb98) + (size_t)(uint32_t)id * 0x23c;
        if (rec) { f148 = c48_rd(rec, 0x148); f14c = c48_rd(rec, 0x14c); f150 = c48_rd(rec, 0x150); f154 = c48_rd(rec, 0x154); }
    }
    int32_t s = c48_scale(f150);
    if (s != 0) {
        g->stamp = tick;
        g->c = (uint32_t)c48_scale(f154);
        g->a = (uint32_t)c48_scale(f14c);
        g->b = (uint32_t)s;
        g->d = f148;
    }
    return 1;
}
}
