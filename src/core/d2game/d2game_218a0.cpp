// Decompile-derived, 1.13c-only. Seams: the D2Common calls the function makes (D2Game thunks +0xA478, +0xA556, +0xADA8, +0xA472, +0xA7B4, +0xA6E2).
#include <cstdint>
#include <cstring>

struct D2Game218a0Env {
    uint32_t (*unitKey)(uint32_t unit);                                         // 0xA478 (#10331)
    uint32_t (*record)(uint32_t key);                                           // 0xA556 (#10826)
    int32_t (*level)(uint32_t rec, uint32_t b, uint32_t d);                     // 0xADA8 (#10894)
    void (*setStatU)(uint32_t unit, uint32_t id, int32_t v, uint32_t sub);      // 0xA472 (#10887)
    uint32_t (*owner)(uint32_t unit, uint32_t flag);                            // 0xA7B4 (#10195)
    void (*setStatO)(uint32_t unit, uint32_t id, int32_t v, uint32_t sub);      // 0xA6E2 (#10188)
};
static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }

// 1.13c D2Game+0x218A0
// ECX = summon-template block (+0x6D byte, +0x70 dword), EDX = unit, two unused stack words (ret 8). Derives a level from the record, sets unit stats 0xC (level)
// and 0x13, then for unit types 0x146..0x171 (four arms) sets the owner's stats 0x15/0x16 and the arm's damage pairs. Non-player unit: fatal exit (not vectored).
extern "C" void d2game_init_summon_stats_218a0(uint32_t blk, uint32_t unit, const D2Game218a0Env* env)
{
    int32_t lvl = 2;
    const uint32_t rec = env->record(env->unitKey(unit));
    if (rec) {
        lvl = env->level(rec, *(const uint8_t*)(uintptr_t)(blk + 0x6d), r32(blk, 0x70));
        if (lvl <= 1) lvl = 1;
    }
    int32_t v = lvl + 0x32;
    if (v >= 0x5a) v = 0x5a;
    env->setStatU(unit, 0xc, lvl, 0);
    env->setStatU(unit, 0x13, v, 0);
    if (!unit || r32(unit, 0) != 1) return;
    const uint32_t o = env->owner(unit, 1);
    const uint32_t t = r32(unit, 4) - 0x146u;
    if (t > 0x2b) return;
    const int32_t h = lvl >> 1;
    const int32_t h3 = (int32_t)((uint32_t)lvl * 3u) >> 1;
    if (t == 0) {
        env->setStatO(o, 0x15, 0, 0); env->setStatO(o, 0x16, 0, 0); env->setStatO(o, 0x30, h, 0); env->setStatO(o, 0x31, h3, 0);
    } else if (t == 1 || t == 2) {
        const int32_t m = h + 1 < lvl ? h + 1 : lvl;
        env->setStatO(o, 0x15, m, 0); env->setStatO(o, 0x16, lvl, 0);
    } else if (t == 3) {
        env->setStatO(o, 0x15, 0, 0); env->setStatO(o, 0x16, 0, 0); env->setStatO(o, 0x39, lvl, 0);
        env->setStatO(o, 0x3a, lvl * 2, 0); env->setStatO(o, 0x3b, lvl * 2, 0);
    } else if (t == 4 || t == 0x2b) {
        env->setStatO(o, 0x15, 0, 0); env->setStatO(o, 0x16, 0, 0); env->setStatO(o, 0x32, h, 0); env->setStatO(o, 0x33, h3, 0);
    }
}
