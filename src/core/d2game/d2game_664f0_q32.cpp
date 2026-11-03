// 0x664F0 (ECX = operate block: +0 game, +4 unit (the stone), +8 player, +0x10 word stone index): A1Q4 Cairn stone operate. Plain ret, eax 0.
// Finds the type-4 quest node (fatal 0x761 when missing); data (node + 0x18) +0x4A unset: 0x633C0 (node). Quest flags (4,0)/(4,1) set: sound 0xDFFD0 (0x13, player), return.
// No 'bkd ' item (0xE660): every 64th try with flags (4,3)/(4,4) clear plays 0x27; +0x2C counts up. Otherwise the stone index must equal the order word at data + 2 * (+0xC);
// the stone id is stored at data + 0x10 + 4 * (+0xC), +0xC increments; the fifth stone sets +0x4F, decrements +0x7C, finds the class-0x15 stone (0xA544) among the placed
// stones or the items of the units in the player's room (0xA478, 0xA55C), spawns through 0xA0000, schedules 0xDFCC0 (0x62A80) / 0x9C160 (0x616F0), sets flag (4,4) and calls 0x35E90.
#include "d2game_q32.h"
using namespace q32;
static uint32_t M(uint32_t rva) { return 0xB2000000u | rva; }
static uint32_t L(const void* p) { return (uint32_t)(uintptr_t)p; }
// 1.13c D2Game+0x664F0. EDI of 0xDFCC0 (the quest node's +4) rides in the s slot.
void d2game_664f0_q32(const Call& X, uint32_t ctx)
{
    const uint32_t g = rd32(ctx, 0), unit = rd32(ctx, 4), pl = rd32(ctx, 8);
    auto fatal = [&](uint32_t line) {   // 0xA2DA, 0xA2C2, 0x2A3C(-1): not driven
        const uint32_t v = X(0xA2DA, 0, 0, 0, 0, 0, line);
        X(0xA2C2, 0, 0, 0, 0, 0, M(0xF9FF9), v);
        X(0x2A3C, 0, 0, 0, 0, 0, 0xffffffffu);
    };
    auto find = [&](uint32_t key) {   // game unit hash: game + 0x1520 + (id & 0x7F) * 4, chain +0xE4, id +0xC
        uint32_t u = rd32(g, 0x1520 + (key & 0x7f) * 4);
        while (u && rd32(u, 0xc) != key) u = rd32(u, 0xe4);
        return u;
    };
    const uint32_t ctl = rd32(g, 0x10f4);
    uint32_t node = ctl ? rd32(ctl, 0) : 0;
    while (node && rd32(node, 0) != 4) node = rd32(node, 0xf4);
    if (!node) { fatal(0x761); return; }
    const uint32_t d = rd32(node, 0x18);
    if (!rd8(d, 0x4a)) { wr8(d, 0x4a, 1); X(0x633C0, 0, 0, 0, 0, 0, node); }
    const uint32_t rec = questRec(X, g, pl);
    if (qGet(X, rec, 4, 0) == 1 || qGet(X, rec, 4, 1) == 1) { X(0xDFFD0, 0, 0, 0, 0, 0, 0x13, pl); return; }
    if (!X(0xE660, pl, 0, 0, 0, 0, 0x20646b62)) {
        if ((rd16(d, 0x2c) & 0x3f) == 0 && qGet(X, rec, 4, 3) == 0 && qGet(X, rec, 4, 4) == 0) X(0xDFFD0, 0, 0, 0, 0, 0, 0x27, pl);
        wr16(d, 0x2c, (uint16_t)(rd16(d, 0x2c) + 1));
        return;
    }
    const uint8_t act = rd8(node, 9), st = rd8(node, 0xc);
    if (!act || st >= 6) return;
    uint32_t link = 0;
    if (rd8(d, 0x49) == 1) {
        if (!g) { fatal(0x2d3); return; }
        link = find(rd32(d, 0x28));
        if (!rd8(d, 0x4e) && link && rd32(link, 4) == 0x3d) {
            if (st == 0 && act == 1) { X(0x635B0, 0, 0, 0, 0, 0, node); X(0x35EB0, 1); }
            if (!X(0xE660, pl, 0, 0, 0, 0, 0x20646b62)) return;
        }
    }
    if (rd8(d, 0x4f) == 1) return;
    if (rd8(node, 0xc) != 5) {
        if (rd8(node, 0xc) == 0 && rd8(node, 9) == 1) { X(0x635B0, 0, 0, 0, 0, 0, node); X(0x35EB0, 1); }
        if (!X(0xE660, pl, 0, 0, 0, 0, 0x20646b62)) return;
        wr8(node, 0xc, 5);
    }
    const uint32_t n = rd16(d, 0xc);
    if (rd16(ctx, 0x10) != rd16(d, n * 2)) return;
    wr32(d, 0x10 + n * 4, unit ? rd32(unit, 0xc) : 0xffffffffu);
    wr16(d, 0xc, (uint16_t)(n + 1));
    const uint32_t cnt = rd16(d, 0xc);
    if (unit && rd32(unit, 0x10)) return;
    if (cnt <= 4) {
        X(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
        if (link) X(0xA4A2, 0, 0, 0, 0, 0, link, rd16(d, 0xc) + 1u);
        return;
    }
    if (cnt != 5) return;
    X(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
    if (link) X(0xA4A2, 0, 0, 0, 0, 0, link, 6);
    wr8(d, 0x4f, 1);
    X(0x366A0, 0x20646b62, g, 0, 0, pl);
    wr32(d, 0x7c, rd32(d, 0x7c) - 1);
    uint32_t out[2] = { 0, 0 }, list = 0, count = 0;
    for (int i = 0; i < 5; i++) {
        if (rd16(d, i * 2) != 0x15) continue;
        if (!g) { fatal(0x2d3); return; }
        const uint32_t u = find(rd32(d, 0x10 + i * 4));
        if (u && rd32(u, 4) == 0x15) X(0xA544, 0, 0, 0, 0, 0, u, L(out));
    }
    if (!out[0]) {
        const uint32_t room = X(0xA478, 0, 0, 0, 0, 0, unit);
        X(0xA55C, 0, 0, 0, 0, 0, room, L(&list), L(&count));
        for (uint32_t i = 0; i < count; i++) {
            bool done = false;
            for (uint32_t it = rd32(rd32(list, i * 4), 0x74); it; it = rd32(it, 0xe8)) {
                if (rd32(it, 0) == 2 && rd32(it, 4) == 0x15) { X(0xA544, 0, 0, 0, 0, 0, it, L(out)); done = true; break; }
            }
            if (done) break;
        }
    }
    if (out[0]) {
        // 0xA0000: ESI player, stack game, 0, 1, 0x120, out[0] + 6, out[1] - 3 (the seam carries four words)
        const uint32_t r = X(0xA0000, 0, 0, 0, 0, pl, g, 0, 1, 0x120);
        if (r) { const uint32_t room2 = X(0xA478, 0, 0, 0, 0, 0, r); X(0xA51A, 0, 0, 0, 0, 0, room2, 0); }
    }
    if (rd8(node, 0xb) < 4) {
        wr8(node, 0x14, 0);
        wr8(node, 0xb, 4);
        X(0xDFCC0, 0, 0, 0, M(0x62a80), rd32(node, 4), pl);
        const uint32_t ax = X(0xDFA60, pl);
        if ((uint16_t)ax != 0xffff) X(0x9C160, ax, 0, 0, 0, g, M(0x616f0));
        qSet(X, rec, 4, 4);
    }
    X(0x35E90, rd32(node, 4), 1);
}
