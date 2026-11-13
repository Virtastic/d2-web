// 1.13c D2Game+0xC8050: a task callback (ECX = game, EDX = event record, plain ret). Record: +0 the owner (its byte +0x6D indexes the per-class word),
// +8, +0xC the unit, +0x14 a short kind, +0x18 a ushort selector. game+0x18 is a state block (bytes +8 +9 +0xA +0xF +0x3C used), game+4 the world owner.
// kind 0xFB / sel 0x12E (only with game+9 set and state+0xF clear): builds an object through 0xE1D90/0xA22E0 and installs three callbacks;
// kind 0xC9: sel 0x1AE enters state 2 (log line, the type-10 node's handler), sel 0x1BA leaves state 3/13; kind 0xD2 sel 0x1C2 leaves state 4;
// anything else (and the 0xFB fall-out) maps sel 0x1BC..0x1C4 to a per-class word write (9 6 7 - 8 - - 0xA ... via a57a).
// Delta against the decompile: Ghidra drops the register inputs (see the header), reads unaffected EBX where the code holds the per-class word, and makes the
// 0xAEE0 byte or 3 land in the low byte of the saved game+4 word (the upper bytes stay game+4's: the code stores the byte into a dword slot).
// The two _exit paths are the fatal seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_c8050.h"

namespace {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 4); return v; }
inline uint32_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline uint32_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(a + o), 2); return v; }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)(a + o), &v, 4); }
inline void wr8(uint32_t a, uint32_t o, uint32_t v) { *(uint8_t*)(uintptr_t)(a + o) = (uint8_t)v; }
const uint32_t kCbState = 0xC5A60, kCb1 = 0xC59A0, kCb2 = 0xC65A0, kCb3 = 0xC4BD0, kCbD2 = 0xC4D30;
}

// 1.13c D2Game+0xC8050
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_c8050_port(const GC8050Env* e, uint32_t game, uint32_t rec) {
    const uint32_t owner = rd32(rec, 0), unit = rd32(rec, 0xC), recW8 = rd32(rec, 8);
    const uint32_t tab = e->a4c0(unit);
    const uint32_t cls = rd32(tab, 0x10 + rd8(owner, 0x6D) * 4);   // the per-class word (EBX in the code)
    const uint32_t st = rd32(game, 0x18);
    const uint32_t kind = rd16(rec, 0x14), sel = rd16(rec, 0x18);
    const uint32_t game4 = rd32(game, 4);
    if (kind == 0xFB) {
        if (rd8(game, 9) != 0 && sel == 0x12E && rd8(st, 0xF) == 0) {
            wr8(st, 0x3C, 1);
            const uint32_t A = e->f1130(unit);
            const uint32_t B = e->f1100(unit);
            const uint32_t room = e->a478(unit);
            if (room == 0) e->fatal(0xE42);
            if (unit != 0 && e->a808(room) != 0) {
                e->dffd0(unit, 0x18, unit);
            } else {
                const uint32_t ch = e->a56e(0x28) & 0xFF;
                const uint32_t a556 = e->a556(room);
                if (ch != (e->a56e(a556) & 0xFF)) e->fatal(0xE5C);
                uint32_t xy[2] = { B, A };
                if (e->ae98(room, xy, 3, 0x3E01, 0xC01, 0) != 0) {
                    const uint32_t x = xy[0], y = xy[1];
                    const uint32_t obj = e->f1280(room);
                    if (obj != 0) {
                        const uint32_t w[7] = { x, y, game4, obj, 1, 1, 0 };
                        const uint32_t nu = e->e1d90(2, 0x3B, w);
                        e->a4a2(nu, 1);
                        const uint32_t w2[4] = { game4, nu, 0x28, a556 };
                        const uint32_t u2 = e->a22e0(w2);
                        if (u2 != 0) {
                            const uint32_t lo = (e->aee0(u2) & 0xFF) | 3;
                            e->aec8(u2, (game4 & ~0xFFu) | lo);   // the byte lands in the low byte of the saved game+4 word
                            e->a51a(obj, 0);
                            e->a51a(e->a478(u2), 0);
                            e->f35eb0(4, game);
                            e->dfcc0(e->fp(kCb1), owner, recW8);
                            e->dfcc0(e->fp(kCb2), owner, recW8);
                            e->dfcc0(e->fp(kCb3), owner, recW8);
                            wr8(st, 0xF, 1);
                            wr8(st, 9, 1);
                            wr32(game, 0xA8, e->fp(kCbState));
                        }
                    }
                }
            }
            wr8(st, 0x3C, 0);
        }
    } else if (kind == 0xC9) {
        if (sel == 0x1AE) {
            e->f35910(recW8, unit, owner);
            wr8(game, 0xC, 2);
            char buf[24]; std::memcpy(buf, "changing state to 2\n", 21);
            if (e->logOn() != 0) e->cbe70((uint32_t)(uintptr_t)buf, game4, 4);
            uint32_t node = 0;
            const uint32_t lst = rd32(game4, 0x10F4);
            if (lst != 0) for (uint32_t n = rd32(lst, 0); n != 0; n = rd32(n, 0xF4)) if (rd32(n, 0) == 10) { node = n; break; }
            wr8(st, 8, 1);
            wr32(game, 0xA8, e->fp(kCbState));
            if (node == 0) return;
            if (e->isBadCode(rd32(game, 0xF0))) e->fatal(0x25A);
            e->nodeCall(node, rd32(node, 0xF0));
            return;
        }
        if (sel != 0x1BA) return;
        if (e->a568(cls, 0xE, 3) == 0) return;
        if (e->a568(cls, 0xE, 0xD) != 0) e->f35eb0(5, game);
        wr8(st, 0xA, 1);
        wr32(game, 0xA8, e->fp(kCbState));
        e->a57a(cls, 0xE, 4);
        e->a586(cls, 0xE, 3);
        e->f35910(recW8, unit, owner);
        return;
    } else if (kind == 0xD2) {
        if (sel != 0x1C2) return;
        if (e->a568(cls, 0xE, 4) == 0) return;
        if (e->a568(cls, 10, 0) != 0) {
            e->f366a0(0x20747368, owner, unit);
            e->f366a0(0x20706976, owner, unit);
            e->f366a0(0x2066736D, owner, unit);
        }
        if (e->a568(cls, 0xE, 0xD) != 0) {
            e->f33920(game4, 0xE, 0xD);
            e->f35eb0(5, game);
            wr8(game, 0x14, 0);
            wr8(game, 0xB, 0xD);
            e->dfcc0(e->fp(kCbD2), game4, 0);
        }
        e->a57a(cls, 0xE, 0);
        e->f34c00(unit, game4);
        e->a586(cls, 0xE, 4);
        e->f33880(game + 0x1C, unit == 0 ? 0xFFFFFFFFu : rd32(unit, 0xC), unit, game);
        return;
    }
    switch (sel) {
    case 0x1BC: e->a57a(cls, 0xE, 9); break;
    case 0x1BD: e->a57a(cls, 0xE, 6); break;
    case 0x1BE: e->a57a(cls, 0xE, 7); break;
    case 0x1BF: e->a57a(cls, 0xE, 0xB); break;
    case 0x1C1: e->a57a(cls, 0xE, 8); break;
    case 0x1C4: e->a57a(cls, 0xE, 10); break;
    }
}
