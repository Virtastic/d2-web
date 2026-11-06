// 1.13c D2Game+0x18CE0: AI think callback (hell meteor family). ECX = game, EDX = unit,
// stack the tick params (+0 AI control, +8 target, +0x14 target distance, +0x18 combat flag, +0x1C monstats-like
// record), ret 4. The record holds the skill id (word +0x170), the cast mode (byte +0x180) and per-difficulty
// (game byte +0x6D) word tables: cast chances +0x56 (out of combat) and +0x5C (in combat), the recast delay +0x74,
// attack chance +0x62, walk chances +0x6E and +0x68. The skill level (0xA3D0/0xA400, 1 when absent or not positive)
// bounds the cast distance. Out of combat, in range, with a free state 12 (0xA68E) and the AI frame passed
// (game +0xA8 > control +0x14): a 0x1180 roll under +0x56 casts (0xF2380, AL = mode, ECX = unit; skill, target, 0, 0)
// and sets the next frame. A set state 12 is cleared (0xA3CA). In combat the same with an inline seed roll under +0x5C;
// then a roll under +0x62 attacks (0xA496 stop, 0x75E10 with an 8-dword command: 4, unit, target, byte +0x15 0x64 or
// 0x65 when the monster record's +0x104 bit is in the imported mask +0x10), a roll under +0x6E walks (mode 5 or 6
// from a further roll's low byte, written to the monster AI block, then 0xF2540(game, unit; target, 2, 0, 0, 1, 0)),
// else 0xF22C0(EAX = game, ESI = unit; 5). Outside combat a roll under +0x68 walks with 7, else 0xF22C0(..., 10).
#ifndef D2G_18CE0_CPP
#define D2G_18CE0_CPP
#include "d2g_mem.h"
namespace d2g_18ce0 {
using namespace d2g;
struct Env {
    P (*skillOf)(P unit, I skill, I m1);           // thunk 0xA3D0: ECX = unit, EDX = skill, stack -1
    I (*skillLevel)(P unit, P sk, I one);          // thunk 0xA400 (stdcall 3)
    I (*state)(P unit, I st);                      // thunk 0xA68E (stdcall 2)
    void (*clearState)(P unit, I st, I v);         // thunk 0xA3CA (stdcall 3)
    U (*rnd)(P seed, I n);                         // 0x1180: EAX = n, ECX = seed
    void (*cast)(uint8_t mode, P unit, I skill, U target, I a, I b);   // 0xF2380: AL = mode, ECX = unit, stack 4
    void (*stop)(P unit, I z);                     // thunk 0xA496 (stdcall 2)
    P (*monRec)(I cls);                            // 0x1320: EAX = class id
    U (*impMask)();                                // dword +0x10 of the import at IAT 0xF8BE0
    void (*attack)(P game, U* cmd, I one);         // 0x75E10 (stdcall 3)
    void (*walk)(P game, P unit, U target, I a, I b, I c, I d, I e);   // 0xF2540: ECX = game, EDX = unit, stack 6
    void (*idle)(P game, P unit, I n);             // 0xF22C0: EAX = game, ESI = unit, stack 1
};
static inline U step(P s) {
    const uint64_t v = (uint64_t)rd(s, 0) * 0x6AC690C5ull + rd(s, 4);
    wr(s, 0, (U)v); wr(s, 4, (U)(v >> 32));
    return (U)v;
}
static inline I tw(P rec, U off, U d) { return (int16_t)rd16(rec, off + 2 * d); }
// 1.13c D2Game+0x18CE0
inline void HellMeteorThink_113c(const Env& e, P game, P unit, P params) {
    P rec = pf(params, 0x1C);
    const I combat = rds(params, 0x18), dist = rds(params, 0x14);
    const U target = rd(params, 8);
    P ctrl = pf(params, 0);
    P seed = at(unit, 0x20);
    const int16_t skill = (int16_t)rd16(rec, 0x170);
    I lvl = 1;
    if (skill >= 0) {
        P sk = e.skillOf(unit, skill, -1);
        if (sk) { const I l = e.skillLevel(unit, sk, 1); if (l > 0) lvl = l; }
    }
    const I st = e.state(unit, 0xC);
    auto castNow = [&](U d) {
        wr(ctrl, 0x14, (U)(tw(rec, 0x74, d) + rds(game, 0xA8)));
        e.cast(rd8(rec, 0x180), unit, skill, target, 0, 0);
    };
    bool toWalk = false;   // c39014
    if (skill >= 0 && !combat && dist < lvl && !st) {
        if (rds(game, 0xA8) > rds(ctrl, 0x14)) {
            const U d = rd8(game, 0x6D);
            if ((I)e.rnd(seed, 100) < tw(rec, 0x56, d)) { castNow(d); return; }
        }
        toWalk = true;
    } else {
        if (st) e.clearState(unit, 0xC, 0);
        if (!combat) toWalk = true;
    }
    if (!toWalk) {
        if (skill >= 0 && rds(game, 0xA8) > rds(ctrl, 0x14)) {
            const U d = rd8(game, 0x6D);
            if ((I)(step(seed) % 100) < tw(rec, 0x5C, d)) { castNow(d); return; }
        }
        if ((I)(step(seed) % 100) < tw(rec, 0x62, rd8(game, 0x6D))) {
            e.stop(unit, 0);
            U cmd[8] = { 4, W(unit), target, 0, 0, 0, 0, 0 };
            U b = 0x64;
            if (unit) {
                P mr = e.monRec(rds(unit, 4));
                if (mr && (rd8(mr, 0x104) & e.impMask())) b = 0x65;
            }
            wr8(cmd, 0x15, (uint8_t)b);
            e.attack(game, cmd, 1);
            return;
        }
        if ((I)(step(seed) % 100) < tw(rec, 0x6E, rd8(game, 0x6D))) {
            const U mode = (step(seed) & 0xFF) < 0x80 ? 5 : 6;
            P ai = (unit && rd(unit, 0) == 1 && rd(unit, 0x14)) ? pf(pf(unit, 0x14), 0x2C) : nullptr;
            wr(ai, 0x18, mode);
            wr8(ai, 0x20, 3);
            e.walk(game, unit, target, 2, 0, 0, 1, 0);
            return;
        }
        e.idle(game, unit, 5);
        return;
    }
    if ((I)(step(seed) % 100) < tw(rec, 0x68, rd8(game, 0x6D))) { e.walk(game, unit, target, 2, 0, 0, 1, 7); return; }
    e.idle(game, unit, 0xA);
}
}
#endif
