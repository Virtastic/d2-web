// 1.13c D2Client+0x23200 --
// stdcall (unit, id) ret 8, void. Plays the UI/unit voice cue `id` for `unit`: picks a sound id from the
// per-type table P = [0xF7694 + unit[1]*4] and submits it to the sound queue (0x88A70, sound id in EBX).
// Callees are D2cHost calls in retail order; 45570 passes six stack words, the log keeps the first four (a4/a5 unpinned).
#include <cstdint>
#include <emscripten.h>

#include "client_host.h"

static uint32_t go(D2cHost h, D2cCall c) { return h(&c); }
static uint32_t w(const uint32_t* p, uint32_t off) { return p[off / 4]; }

// tbl = the 7 pointers at D2Client+0xF7694 (each a retail struct P: +8..+0x1C ids, +0x20/+0x24 bases);
// player = the struct at [D2Client+0x11BBFC] (its +0x10 is read), tick = [0x1197F8],
// last = {[0x11C4CC], [0x11C4D0]} (read and written), base = D2Client image base (error string at +0xD359F).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_23200(D2cHost h, uint32_t uUnit, const uint32_t* unit, uint32_t id16,
                                                     const uint32_t* const* tbl, uint32_t uPlayer, const uint32_t* player,
                                                     uint32_t tick, uint32_t* last, uint32_t base) {
    if (!unit || unit[1] > 6) {
        uint32_t m = go(h, { 0xBF92, 0, 0, 0, 0, 0, { 0xB22, 0, 0, 0 } });
        go(h, { 0xBF80, 0, 0, 0, 0, 0, { base + 0xD359F, m, 0, 0 } });
        go(h, { 0x3664, 0, 0, 0, 0, 0, { 0xFFFFFFFF, 0, 0, 0 } });
        return;
    }
    const uint32_t* P = tbl[unit[1]];
    if (!P) return;
    uint32_t id = id16 & 0xFFFF, s0 = uUnit, s1 = 0, esi = 0, ebx = 0;
    bool dedupe = false;
    if (uUnit == uPlayer && (!player || w(player, 0x10) == 0x11)) return;
    auto muted = [&] { return go(h, { 0x87BD0, 0, 0, 0, uUnit, 0, { 0, 0, 0, 0 } }) != 0; };

    if (id >= 0x19 && id <= 0x20 && !muted()) esi = w(P, 0x20) + id * 2 - 0x32;
    if (id >= 0x21 && id <= 0x53) {
        ebx = w(P, 0x24) + id - 0x21;
        struct S { uint32_t id, a1, a2, a4, a5; };
        static const S sp[] = { {0x23,0x1250,0,0x12C,0xFA}, {0x22,0x124E,0x1DB,0x320,0x20D}, {0x25,0,0,0x5DC,0},
            {0x21,0x124D,0,0x1DB,0x1A9}, {0x32,0x1255,0x1DB,0x44C,0x20D}, {0x34,0x1256,0,0x1A9,0x177},
            {0x42,0x1254,0,0x1A9,0x177}, {0x4B,0x1257,0,0x1DB,0x145}, {0x50,0x125A,0,0x1F4,0x1DB},
            {0x52,0x1259,0,0x226,0x190}, {0x53,0x1258,0,0x20D,0x1A9} };
        for (const S& e : sp) if (e.id == id) {
            go(h, { 0x45570, 0, 0, 0, 1, 0, { e.a1, e.a2, e.a4, ebx } });  // stack words (a1, 0, a4, id, a5, 1): first four logged
            return;
        }
        static const uint32_t zero[] = {0x26,0x28,0x29,0x2A,0x2B,0x2C,0x2E,0x2F,0x37,0x38};
        esi = ebx; s1 = 0xC; for (uint32_t z : zero) if (z == id) s1 = 0;
        goto submit;
    }
    switch (id) {
    case 1: esi = 0xEB; break;
    case 2: esi = 7; s0 = 0; break;
    case 3: esi = 0xA; s0 = 0; break;
    case 4: esi = 0xB; s0 = 0; break;
    case 5: esi = 0xC; s0 = 0; break;
    case 6: esi = 0xD; s0 = 0; break;
    case 7: esi = 0x8B6; break;
    case 8: esi = 0x8B7; break;
    case 9: esi = 9; break;
    case 0xB: esi = 0xE4; break;
    case 0x13: case 0x14: case 0x15: case 0x16: case 0x17: case 0x18: {
        static const uint32_t off[] = {8, 0x18, 0xC, 0x10, 0x14, 0x1C};
        esi = w(P, off[id - 0x13]); dedupe = true;
        if (muted()) return;
        break; }
    default: break;
    }
    if (esi == 0) return;
    if (dedupe) {
        if (esi == last[1] && (uint32_t)(tick - last[0]) < 0x4B) return;
        last[0] = tick; last[1] = esi;
    }
submit:
    uint32_t r = go(h, { 0x88A70, 0, 0, 0, esi, 0, { s0, s1, 0, 0 } });
    r = go(h, { 0x87E60, 0, 0, r, 0, 0, { 0, 0, 0, 0 } });
    go(h, { 0x7C270, uUnit, 0, 0, 0, r, { 0, 0, 0, 0 } });
}
