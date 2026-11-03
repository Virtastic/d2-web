// SUNITINACTIVE_CompressUnitIfNeeded (1.13c D2Game+0x20760): EAX = unit, stack = game (ret 4). Retail-layout (32-bit offsets) rule; every engine callee is an Env member.
// Decides whether the unit goes into the inactive store (record kept via 0x204E0) and then removes it from the room (0xE20A0), or, when the unit
// still has a pending corpse/death hook (0x98070 result), only marks it (+0xC8 |= 0x100) and releases it (thunk 0xB354).
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G20760Env {
    uint32_t dataTables;                                        // *sgptDataTables (slot 0xF829C): +0xA78 monstats (0x1A8), +0xA80 count, +0xA90 hireling/class table (0x134), +0xA98 count
    uint32_t (*room)(uint32_t unit);                            // thunk 0xA478 (#10331)
    uint32_t (*roomLevel)(uint32_t room);                       // thunk 0xA556 (#10826)
    uint32_t (*levelIsTown)(uint32_t level);                    // thunk 0xB35A (#10929)
    uint32_t (*unitFlagA)(uint32_t unit);                       // thunk 0xAD60 (#10541)
    uint32_t (*unitMode)(uint32_t unit);                        // thunk 0xA514 (#10830)
    void (*e0460)(uint32_t unit, uint32_t one, uint32_t roomLow);   // 0xE0460 (EAX unit, EBX = room with low byte cleared, stack 1)
    void (*efc60)(uint32_t game);                               // 0xEFC60 (EDI = game)
    uint32_t (*e03a0)(uint32_t eax, uint32_t ecx, uint32_t edx);    // 0xE03A0
    uint32_t (*sub98070)(uint32_t unit, uint32_t game);         // 0x98070 (EAX unit, stack game)
    void (*compress)(uint32_t unit, uint32_t game);             // 0x204E0
    void (*release)(uint32_t unit);                             // thunk 0xB354 (#10650)
    void (*e20a0)(uint32_t game, uint32_t unit);                // 0xE20A0 (ECX game, EDX unit)
};

// 1.13c D2Game+0x20760
void SUNITINACTIVE_CompressUnitIfNeeded(uint32_t unit, uint32_t game, const D2G20760Env& env)
{
    const uint32_t room = env.room(unit);
    const uint32_t mode = rd32(unit + 0x10), type = rd32(unit);
    const bool g = (rd8(unit + 0xC6) & 1) || (type == 0 ? (mode == 0 || mode == 0x11) : type == 1 ? (mode == 0 || mode == 0xC) : true);
    bool want = false;
    if (env.levelIsTown(env.roomLevel(room))) want = true;
    if (rd32(unit + 0xC4) & 0x2000000) want = true;
    if (mode == 0xC) {
        const uint32_t p = room ? room + 0x6C : 0;
        const uint64_t v = (uint64_t)rd32(p) * 0x6AC690C5u + rd32(p + 4);
        wr32(p, (uint32_t)v); wr32(p + 4, (uint32_t)(v >> 32));
        if ((uint32_t)v % 3) want = false;
    }
    if (g) {
        if (env.unitFlagA(unit)) want = false;
        if (env.unitMode(unit) == 2) want = false;
    }
    bool skipTail = false;
    if ((int32_t)rd32(unit + 0xD0) < 8) {
        if (!g) { want = true; env.e0460(unit, 1, room & ~0xFFu); env.efc60(game); skipTail = true; }
        else want = false;
    }
    if (!skipTail && g) { const uint32_t c = rd32(unit + 4); if (c == 0x16B || c == 0x16C) want = false; }
    const uint32_t pd = type == 1 ? rd32(unit + 0x14) : 0;
    if (pd && (rd8(pd + 0x16) & 0x18) && g) want = false;
    uint32_t pending = 0;
    if (rd32(unit + 0xC4) >> 31) {
        if (pd) {
            const uint32_t ai = rd32(pd + 0x28);
            if (ai && rd32(ai + 0x28)) {
                const uint32_t q = env.e03a0(rd32(ai + 0x30), rd32(ai + 0x28), rd32(ai + 0x2C));
                if (q && rd32(q) == 0) pending = env.sub98070(unit, game);
            }
        }
        want = false;
    }
    if (rd32(unit + 0xC4) >> 9 & 1) want = false;
    const int32_t cls = (int32_t)rd32(unit + 4);
    const uint32_t dt = env.dataTables;
    bool ok = false;
    if (cls >= 0 && cls < (int32_t)rd32(dt + 0xA80)) {
        const int32_t idx = (int16_t)rd16(rd32(dt + 0xA78) + (uint32_t)cls * 0x1A8 + 0x18);
        if (idx >= 0 && idx < (int32_t)rd32(dt + 0xA98)) {
            const uint32_t rec = idx * 0x134u + rd32(dt + 0xA90);
            if (rec) { const uint8_t b = rd8(rec + 0x130); if (b) { ok = true; if (b == 2) want = true; } }
        }
    }
    if (!ok) want = false;
    if (pending) {
        wr32(unit + 0xC8, rd32(unit + 0xC8) | 0x100);
        if (want) env.compress(unit, game);
        env.release(unit);
        return;
    }
    if (want) env.compress(unit, game);
    env.e20a0(game, unit);
}
