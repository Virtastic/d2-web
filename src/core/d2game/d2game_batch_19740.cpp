// 1.13c D2Game+0x19740 -- AI think helper: given a monster unit (type 1), pick a target and either
// idle, use the hardcoded mode/skill, walk close, or fall back to a seed-rolled retreat.
// Callees are seams in retail argument order; unit seed is the dword pair at +0x20/+0x24.
#include <cstdint>
#include <cstring>
namespace d2game_batch19740 {
using U = uint32_t; using P = void*;
struct Env {
    U (*unitClassOrId)(P);            // Ordinal 10331
    int (*isExcluded)(U);             // Ordinal 10057
    P (*findTarget)(P game, int* outDist);   // B1BC0
    U (*roll100)(U* seed);            // 0x1180 (seed = unit+0x20)
    void (*idle)(U frames);           // F22C0 (game/unit implicit)
    void (*modeTarget)(P game, P target);    // F2220
    void (*useSkill)(P unit);         // F2380
    void (*skillMode)(P unit, P game, P target, int mode); // F26B0
    void (*walkClose)(P game, P unit);       // F1B10
};
// 1.13c D2Game+0x19740
void sub_19740(const Env& e, P game, P unit) {
    uint8_t* u = (uint8_t*)unit;
    U type; if (u) std::memcpy(&type, u + 0x10, 4);
    if (!u || type != 1) { e.idle(5); return; }
    U* seed = (U*)(u + 0x20);
    P target = nullptr; int dist = 0;
    if (e.isExcluded(e.unitClassOrId(unit)) == 0) {
        target = e.findTarget(game, &dist);
        if (target && dist < 0x14) {
            if ((int)e.roll100(seed) < 0x1e) {
                U cls; std::memcpy(&cls, u + 4, 4);
                if (cls == 0x10f) e.useSkill(unit); else e.modeTarget(game, target);
                return;
            }
            if ((int)e.roll100(seed) < 0x1e) { e.skillMode(unit, game, target, 4); return; }
            e.idle(10); return;
        }
    }
    uint64_t v = (uint64_t)seed[0] * 0x6ac690c5u + seed[1];
    std::memcpy(seed, &v, 8);
    if ((uint32_t)v % 100 < 0x14) { e.walkClose(game, unit); return; }
    e.idle(10);
}
}
