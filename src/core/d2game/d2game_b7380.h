// 1.13c D2Game+0xB7380 (AIBAAL_RollRandomAiParamForNonCollidingUnit): EAX = pUnit, stack = (pGame, pAiControl, target, count,
// inMedium, inFar, inClose, nMax), ret 0x20. Same weight table as the vendored 1.10f function; the 1.13c register convention is
// pinned from the disassembly (callees 0xB0A20 EBX = unit, 0xB6C90 ESI = skill owner, 0xAF840 ECX = unit, ESI = out block, EDI = game).
// Rule over an environment E (d2game_c310.h): every non-field callee is one E::h call.
#pragma once
#include <cstdint>

namespace g7380 {
template <class E> uint32_t Roll(E& e, uint32_t unit, uint32_t game, uint32_t ai, uint32_t tgt, int32_t count, uint32_t med,
                                 uint32_t far_, uint32_t close, int32_t nmax)
{
    int32_t w[16] = { 0, 0, 5, 5, 5, 0, 5, 0, 40, 40, 0, 70, 80, 60, 20, 0 };
    int32_t bonus = 100;
    if (E::rd8(game + 0x6D) == 0) { bonus = 125; w[13] = 40; }
    const int32_t c = (2 - (int32_t)e.rd32(ai + 0x18)) * 10;
    w[15] = c < 0 ? 0 : c;
    const int32_t dist = (int32_t)e.h(0xB0A20, 0, 0, 0, unit, 0, tgt);
    if (dist > 0x23) { w[6] = 15; w[13] = 0; }
    const int32_t life = (int32_t)e.h(0xA718, 0, 0, 0, 0, 0, unit);
    w[1] = bonus + (100 - life);
    if (dist > 0x19) { w[14] = 30; w[11] = 0; }
    if (count < 2) w[11] -= 10;
    if (count > 3) w[13] += 25;
    if (nmax > 0x3C) w[8] = 70;
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    uint32_t out[5] = {};   // 0xAF840 fills the player-count block through ESI
    e.h(0xAF840, room, unit, 0, 0, e.stk(out));
    e.edi(0xAF840, game);
    if (count < 2 && (int32_t)out[3] < 2) w[8] = 0;
    uint32_t sk = e.h(0xA3B2, 0, 0, 0, 0, 0, tgt);
    bool prime = e.h(0xB6C90, 0, 0, 0, 0, sk) != 0;
    if (!prime) {
        sk = e.h(0xA3AC, 0, 0, 0, 0, 0, tgt);
        prime = e.h(0xB6C90, 0, 0, 0, 0, sk) != 0;
    }
    if (prime && !med) { w[6] += 30; w[11] += 10; w[14] += 15; }
    else if (med) { w[6] += 25; w[3] = 25; w[2] = 0; w[14] += 35; }
    if (far_) w[5] = 60;
    if (close) w[7] = 0;
    int32_t sum = 0;
    for (int i = 0; i < 16; i++) sum += w[i];
    uint32_t r = 0;
    if (sum > 0) {
        const uint64_t v = (uint64_t)e.rd32(unit + 0x20) * 0x6ac690c5ull + e.rd32(unit + 0x24);
        E::wr32(unit + 0x20, (uint32_t)v);
        E::wr32(unit + 0x24, (uint32_t)(v >> 32));
        r = ((uint32_t)sum & ((uint32_t)sum - 1)) == 0 ? ((uint32_t)sum - 1) & (uint32_t)v : (uint32_t)v % (uint32_t)sum;
    }
    int32_t acc = 0;
    for (uint32_t i = 0; i < 16; i++) { acc += w[i]; if ((int32_t)r < acc) return i; }
    return 1;
}
}
