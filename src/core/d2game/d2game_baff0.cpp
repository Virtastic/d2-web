// src/core/d2game/d2game_baff0.cpp -- 1.13c D2Game.dll sub_BAFF0 (spawns the 0x236 monster-class unit next to a unit and marks the caller's record),
// transcribed from the cached decompile resolved against its disassembly (register inputs, out-parameter slots and stack argument order are read from
// the assembly). Callees go through the ord/sub seams of D2GameBaff0Env (ord = D2Common/Fog ordinal, sub = D2Game RVA), arguments in retail stack order.
#include <cstdint>
#include <emscripten.h>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define R8(b, o)  (*(uint8_t*)((uintptr_t)(b) + (o)))

struct D2GameBaff0Env {
    S (*ord)(U n, U a, U b, U c, U d, U e, U f, U g);
    S (*sub)(U rva, U a, U b, U c, U d, U e, U f, U g, U h);   // (EAX, ECX, EDX, then the stack words in push order)
};
static const D2GameBaff0Env* G;
static inline S o(U n, I a = 0, I b = 0) { return G->ord(n, (U)a, (U)b, 0, 0, 0, 0, 0); }
static inline U P(const void* p) { return (U)(uintptr_t)p; }

// 1.13c D2Game+0xBAFF0 -- EAX = ctx {game, ?, unit}, stack = (the record whose bytes +0x44/+0x45 are set). ret 4.
// Returns the EAX retail leaves: 0x34130's result when no cell is found, 0 when the unit allocation fails, else the record.
extern "C" EMSCRIPTEN_KEEPALIVE U d2_fn_d2game_BAFF0(I ctx, I arg, const D2GameBaff0Env* env)
{
    G = env;
    const I unit = R32(ctx, 8);
    U w[5] = {0, 0, 0, 0, 0};   // w0 = the room Ord10331 hands back (then the search result), w3/w4 = the unit's position pair
    o(10141, unit, P(&w[3]));
    w[3] += 5;
    w[0] = (U)o(10331, unit);
    // (room, &pos, 2, 0x400, &w0, 100): fills w0 with the found cell, 0 when nothing is free
    const U r34 = (U)G->sub(0x34130, (U)o(10331, unit), P(&w[3]), 2, 0x400, P(&w[0]), 100, 0, 0);   // stack words only
    const U found = w[0];
    if (!found) return r34;
    const I game = R32(ctx, 0);
    w[1] = w[3]; w[2] = w[4];
    const I m = o(11153, R32(game, 0x1c), 2);
    if (!m) return 0;
    R32(m, 0) = 2; R32(m, 4) = 0x236; R32(m, 8) = R32(game, 0x1c); R32(m, 0x80) = (U)game;
    U fl = R32(m, 0xc8);
    fl = R32(game, 0x70) ? (fl | 0x2000000) : (fl & 0xfdffffff);
    R32(m, 0xc8) = fl;
    R32(m, 0xc8) = fl | 0x4000000;
    const U cls = (uint8_t)o(10459, o(10826, found));
    R8(m, 0x18) = (uint8_t)cls;
    R32(m, 0x1c) = R32(game, 0xbc + cls * 4);
    R32(m, 0x74) = 0;
    G->sub(0xE0110, (U)(game + 0xd0), (U)m, 0, 0, 0, 0, 0, 0);   // (ESI = game + 0xD0, EDI = the new unit)
    R32(m, 0xc4) |= 0x10;
    R32(m, 0xd0) = 0xb;
    U gen = R32(game, 0x98) + 1;
    if (gen == 0xffffffffu) gen = 1;
    R32(game, 0x98) = gen;
    // Fog #10045 (ECX = game[0x1C], EDX = 0x38, stack = (file, line, 0))
    const I z = G->sub(0xA2E6, 0, R32(game, 0x1c), 0x38, 0xFB02C, 0x265, 0, 0, 0);
    R32(m, 0x14) = (U)z;
    for (int i = 0; i < 0xe; ++i) R32(z, i * 4) = 0;
    R32(R32(m, 0x14), 0) = (U)o(10688, 0x236);
    R32(m, 0x10) = 1;
    o(10445, m);
    G->sub(0x2F300, (U)m, 0, 0, gen, w[0], w[1], w[2], 0);   // (ESI = m, 0, 0, stack = (gen, w0, w1, w2))
    G->sub(0xE0F40, w[0], w[2], (U)m, w[1], (U)game, 1, 0, 0);   // EAX = w0, ECX = w2, EDX = m, stack = (w1, game, 1)
    R8(arg, 0x44) = 0; R8(arg, 0x45) = 1;
    R32(m, 0xc4) |= 0x3000000;
    return (U)arg;
}
