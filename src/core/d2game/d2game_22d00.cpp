// Decompile-derived, 1.13c-only. Seams: the percent-bonus helper (D2Game+0x21EC0), the skill-table lookup (#10237, D2Game+0xA7BA), the two
// state probes (D2Game+0x18A0, +0x1850) and the stat set (#10551, D2Game+0xA4D2).
#include <cstdint>
#include <cstring>

struct D2Game22d00Env {
    void (*bonus)(uint32_t unit, int32_t pct);                    // 0x21EC0: EBX = unit, EDI = pct
    uint32_t (*record)(uint32_t id);                              // 0xA7BA (#10237)
    uint32_t (*probeA)(uint32_t unit);                            // 0x18A0: EAX = unit
    uint32_t (*probeB)(uint32_t a);                               // 0x1850: EAX = previous result
    void (*setStat)(uint32_t unit, uint32_t id, int32_t v, uint32_t sub);   // 0xA4D2
};
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }

// 1.13c D2Game+0x22D00
// ECX = unit, stack arg (ret 4). For a player unit (type 1) with a +0x80 block and a nonzero arg: runs the 0x21EC0 bonus with percent -75, looks the
// record of byte +0x6D up and derives a value from its +0x34 field (x 300 / 100, halved when the 0x1850 probe says 0x76; 300 when there is no record),
// then sets stat 0x19 to the (possibly halved) value and stat 0x77 to the full one.
extern "C" void d2game_set_speed_stats_22d00(uint32_t unit, uint32_t arg, const D2Game22d00Env* env)
{
    if (!unit || r32(unit, 0) != 1) return;
    const uint32_t blk = r32(unit, 0x80);
    if (!blk || !arg) return;
    env->bonus(unit, -75);
    int32_t half = 300, full = 300;
    const uint32_t rec = env->record(*(const uint8_t*)(uintptr_t)(blk + 0x6d));
    if (rec) {
        const int32_t p = (int32_t)(r32(rec, 0x34) * 300u);
        full = half = p / 100;
        const uint32_t a = env->probeA(unit);
        if (env->probeB(a) == 0x76) half = half / 2;
    }
    env->setStat(unit, 0x19, half, 0);
    env->setStat(unit, 0x77, full, 0);
}
