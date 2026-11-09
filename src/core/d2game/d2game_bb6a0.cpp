// src/core/d2game/d2game_bb6a0.cpp -- 1.13c D2Game.dll sub_BB6A0 (an act quest's per-tick roll: sets the unit's +0xB8 word from the 4-char tag table, then
// calls sub_31A70), transcribed from the cached decompile resolved against its disassembly. Callees go through the ord/sub seams of D2GameBb6a0Env
// (ord = D2Common/Fog ordinal, stack words in push order; sub = D2Game RVA, (EAX, ECX, EDX, then the stack words in push order)).
// `tbl` is the module's tag table at D2Game+0x106F0C (4-char tags "r01 " ... "sku ").
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define R8(b, o)  (*(uint8_t*)((uintptr_t)(b) + (o)))

struct D2GameBb6a0Env {
    S (*ord)(U n, U a, U b, U c, U d, U e, U f, U g);
    S (*sub)(U rva, U a, U b, U c, U d, U e, U f, U g, U h);
    const uint8_t* tbl;
};

// 1.13c D2Game+0xBB6A0 -- ESI = unit, stack = (quest node). ret 4.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_BB6A0(I unit, I node, const D2GameBb6a0Env* e)
{
    const I st = R32(node, 0x18);
    if (R8(st, 4)) e->ord(11090, (U)unit, 4, 0, 0, 0, 0, 0);
    if (!R8(st, 3) || (S)R32(st, 0x14) <= 0) return;
    U arg = 0x32, hits = 0;
    const I game = R32(node, 4);
    auto fatal = [&] {   // 0xA2DA(0x1FB), 0xA2C2(msg 0x6FD19FF9, res), all stack words, 0x22A3C(-1): the process exits
        const U r = (U)e->sub(0xA2DA, 0x1fb, 0, 0, 0, 0, 0, 0, 0);
        e->sub(0xA2C2, 0x6fd19ff9u, r, 0, 0, 0, 0, 0, 0);
        e->sub(0x22A3C, 0xffffffffu, 0, 0, 0, 0, 0, 0, 0);
    };
    for (S i = 0; i < (S)R32(st, 0x14); ++i) {
        U off;
        switch (R32(st, 8)) {
        case 1: off = 0xbc; break;
        case 2: case 3: off = 0xa0; break;
        case 4: off = 0x84; break;
        default: return;
        }
        const I sd = R32(game, 0x10f4);
        if (!sd) { fatal(); return; }
        const uint64_t s = (uint64_t)R32(sd, 0x18) * 0x6ac690c5ull + R32(sd, 0x1c);
        R32(sd, 0x18) = (U)s; R32(sd, 0x1c) = (U)(s >> 32);
        R32(unit, 0xb8) = R32(e->tbl, (U)s % 7 * 4 + off);
        // stack words only: (unit, 2, &arg, 0, -1, 0)
        if (e->sub(0x31A70, (U)unit, 2, (U)(uintptr_t)&arg, 0, 0xffffffffu, 0, 0, 0)) ++hits;
    }
    if (!hits) return;
    const S left = (S)R32(st, 8) - 1;
    R32(st, 8) = (U)left;
    if (left >= 1) {
        // ECX = game, EDX = unit, stack = (7, game[0xA8] + 0x14, 0, 0, 0)
        e->sub(0x8EA80, 0, (U)game, (U)unit, 7, R32(game, 0xa8) + 0x14, 0, 0, 0);
        return;
    }
    R8(st, 3) = 0;
    if (!R32(game, 0x70)) return;
    U base = 0;
    if (R8(game, 0x6d) == 1) base = 0x2c; else if (R8(game, 0x6d) == 2) base = 0x58;
    const I sd = R32(game, 0x10f4);
    if (!sd) { fatal(); return; }
    const U r = (U)e->sub(0x21180, 0xb, (U)(sd + 0x18), 0, 0, 0, 0, 0, 0);   // EAX = 11 (modulus), ECX = &seed
    R32(unit, 0xb8) = R32(e->tbl, base + r * 4);
    e->sub(0x31A70, (U)unit, 2, (U)(uintptr_t)&arg, 0, 0xffffffffu, 0, 0, 0);
}
