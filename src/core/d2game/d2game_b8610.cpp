// src/core/d2game/d2game_b8610.cpp -- 1.13c D2Game.dll AITHINK_Fn135_BaalCrab (sub_B8610), transcribed from the cached decompile resolved
// against its disassembly. Callees go through the ord/sub seams of D2GameBatchB8610Env (ord = D2Common ordinal, sub = D2Game RVA).
// sub() argument order per callee is documented at each call: (register args, then the stack words in retail push order).
#include <cstdint>
#include <emscripten.h>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))

struct D2GameBatchB8610Env {
    S (*ord)(U n, U a, U b, U c, U d, U e, U f, U g);
    S (*sub)(U rva, U a, U b, U c, U d, U e, U f, U g, U h);
};
static const D2GameBatchB8610Env* G;
static inline U P(const void* p) { return (U)(uintptr_t)p; }

// type-10 record in the player's +0x14, +0x28 data block: circular list whose head is the pointer at +0x20
static I find10(I u)
{
    if (R32(u, 0) != 1) return 0;
    I pd = R32(u, 0x14);
    if (!pd) return 0;
    I blk = R32(pd, 0x28);
    if (!blk) return 0;
    I head = R32(blk, 0x20);
    if (!head) return 0;
    I p = R32(head, 0);
    while (R32(p, 8) != 10 && p != head) p = R32(p, 0);
    return R32(p, 8) == 10 ? p : 0;
}
// types 2, 4 and 5 read the record at +0x2C directly (+0xC x, +0x10 y); others ask Ord10867 / Ord10750 (null record = 0)
static inline bool direct(U t) { return t == 2 || ((S)t > 3 && (S)t <= 5); }

// 1.13c D2Game+0xB8610 -- ECX = game, EDX = unit, stack = (&command word). ret 4.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_B8610(I game, I unit, I cmdp, const D2GameBatchB8610Env* env)
{
    G = env;
    U a = 0, b = 0;
    // (game, unit, &a, &b, 0, callback 0xB6D30): returns the value kept in the frame
    U c = (U)G->sub(0xB7090, (U)game, (U)unit, P(&a), P(&b), 0, 0xB6D30, 0, 0);
    I ev = find10(unit);
    if (!ev) {
        U rec[5] = {0, 0, 10, 0, 0};
        const U t = R32(unit, 0), p = R32(unit, 0x2c);
        rec[3] = direct(t) ? R32(p, 0xc) : (p ? (U)G->ord(10867, p, 0, 0, 0, 0, 0, 0) : 0);
        rec[4] = direct(t) ? R32(p, 0x10) : (p ? (U)G->ord(10750, p, 0, 0, 0, 0, 0, 0) : 0);
        // EAX = game, ECX = unit, stack = (&rec), the record starting two words below its type word
        G->sub(0x21290, (U)game, (U)unit, 0, P(rec), 0, 0, 0, 0);
        ev = find10(unit);
    }
    const U cmd = R32(cmdp, 0);
    // EAX = c, ECX = unit, stack = (game, cmd, a, b, ev)
    U r = (U)G->sub(0xB79B0, c, (U)unit, 0, (U)game, cmd, a, b, (U)ev);
    // EAX = c, ECX = unit, EDX = r, stack = (game, cmd, ev)
    G->sub(0xB8070, c, (U)unit, r, (U)game, cmd, (U)ev, 0, 0);
    for (I n = R32(unit, 0xdc); n;) {
        const uint8_t t = *(uint8_t*)(uintptr_t)n;
        const I nx = R32(n, 0x24);
        if (t == 2) G->sub(0x8E6F0, (U)n, (U)game, 0, 0, 0, 0, 0, 0);   // EAX = node, EDI = game
        n = nx;
    }
    // ECX = game, EDX = unit, stack = (2, game[0xA8] + 0x19, 0, 0, 0)
    G->sub(0x8EA80, 0, (U)game, (U)unit, 2, R32(game, 0xa8) + 0x19, 0, 0, 0);
}
