// src/core/d2game/d2game_b7d30.cpp -- 1.13c D2Game.dll CreateProjectileFromSkill (sub_B7D30), transcribed from the cached decompile
// resolved against its disassembly (register inputs, out-parameter slots and the x/y roll order are read from the assembly).
// Callees go through the ord/sub seams of D2GameBatchB7D30Env (ord = D2Common ordinal, sub = D2Game RVA), arguments in retail stack order.
#include <cstdint>
#include <emscripten.h>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define S32(b, o) (*(int32_t*)((uintptr_t)(b) + (o)))
#define R8(b, o)  (*(uint8_t*)((uintptr_t)(b) + (o)))
#define R16(b, o) (*(uint16_t*)((uintptr_t)(b) + (o)))

struct D2GameBatchB7D30Env {
    S (*ord)(U n, U a, U b, U c, U d, U e, U f, U g);              // D2Common ordinal, stack words in order
    S (*sub)(U rva, U a, U b, U c, U d, U e, U f, U g, U h);       // D2Game callee by RVA
    const uint8_t* dt;                                             // *sgptDataTables_exref
};
static const D2GameBatchB7D30Env* G;
static inline S o(U n, I a = 0, I b = 0, I c = 0, I d = 0, I e = 0, I f = 0, I g = 0) { return G->ord(n, (U)a, (U)b, (U)c, (U)d, (U)e, (U)f, (U)g); }

// Anchor coordinates: types 2, 4 and 5 read the record at +0x2C directly (+0xC x, +0x10 y); others ask Ord10867 / Ord10750 (null record = 0).
static inline S coordX(I u) { U t = R32(u, 0); I p = R32(u, 0x2c); if (t == 2 || (t > 3 && t <= 5)) return S32(p, 0xc); return p ? o(10867, p) : 0; }
static inline S coordY(I u) { U t = R32(u, 0); I p = R32(u, 0x2c); if (t == 2 || (t > 3 && t <= 5)) return S32(p, 0x10); return p ? o(10750, p) : 0; }
// A player unit's data block (+0x14, then +0x28): flag bit 0 in word +8, then (id, type, game) at +0x2C/+0x30/+0x28.
static inline void tagLink(I u, U id, U type, I game) {
    if (R32(u, 0) != 1) return;
    I pd = R32(u, 0x14);
    uintptr_t blk = pd ? R32(pd, 0x28) : 0;
    if (!blk) return;
    R16(blk, 8) |= 1; R32(blk, 0x2c) = id; R32(blk, 0x30) = type; R32(blk, 0x28) = (U)game;
}

// 1.13c D2Game+0xB7D30 -- EAX = the casting unit (esi), stack = (game, counter record). ret 8.
// Returns 1 when a projectile unit was created and linked, 0 otherwise.
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2game_B7D30(I unit, I game, I counter, const D2GameBatchB7D30Env* env)
{
    G = env;
    U slot = (U)unit, flag = 0;
    G->sub(0x21490, (U)unit, (U)(uintptr_t)&slot, (U)(uintptr_t)&flag, 0, 0, 0, 0, 0);
    if (flag) return 0;
    if (unit && ((R32(unit, 0xc8) >> 10) & 1) && G->sub(0xE03A0, (U)game, R32(unit, 0x98), R32(unit, 0x94), 0, 0, 0, 0, 0) != 0)
        if (G->sub(0xDFA90, 0, 0, 0, 0, 0, 0, 0, 0) == 0) return 0;
    U cls = (S32(G->dt, 0xa80) > 0x23a) ? 0x23a : 0xFFFFFFFFu, one = 1, outY = 0, outX = 0;
    // (unit, &cls, &outY, &outX, &one, the game byte +0x6D, 0): the out slots are stack locals of the retail frame
    o(10751, unit, (U)(uintptr_t)&cls, (U)(uintptr_t)&outY, (U)(uintptr_t)&outX, (U)(uintptr_t)&one, R8(game, 0x6d), 0);
    G->sub(0xE07A0, (U)game, (U)unit, 0, 0, 0, 0, 0, 0);
    I owner = o(10392, R32(unit, 0x2c));
    I anchor = (owner && owner != unit) ? owner : unit;
    const S x = coordX(anchor), y = coordY(anchor);
    // Two LCG steps on the 64-bit seed at +0x20: v = lo * 0x6AC690C5 + hi; x and y are rolled mod 24 around the anchor.
    uint64_t v1 = (uint64_t)R32(unit, 0x20) * 0x6ac690c5ull + R32(unit, 0x24);
    const S xr = (S)((U)v1 % 0x18) + x - 0xc;
    uint64_t v2 = (uint64_t)(U)v1 * 0x6ac690c5ull + (U)(v1 >> 32);
    *(uint64_t*)((uintptr_t)unit + 0x20) = v2;
    const S yr = (S)((U)v2 % 0x18) + y - 0xc;
    // GetRoom runs twice with the same arguments (EAX = Ord10331's value, EBX = x, EDI = y); retail keeps the first result.
    I room = G->sub(0x1280, (U)o(10331, unit), (U)xr, (U)yr, 0, 0, 0, 0, 0);
    if (!room) return 0;
    G->sub(0x1280, (U)o(10331, unit), (U)xr, (U)yr, 0, 0, 0, 0, 0);
    I np = G->sub(0xEF650, cls, 1, (U)game, (U)room, (U)xr, (U)yr, (U)-1, 0);
    if (!np) return 0;
    R32(np, 0xc4) |= 0x4020000;
    G->sub(0xF20C0, (U)game, (U)np, 0xf, 0, 0, 0, 0, 0);
    tagLink(unit, R32(unit, 0xc), R32(unit, 0), game);
    G->sub(0x20CF0, (U)unit, (U)np, (U)game, 0, 0, 0, 0, 0);
    tagLink(np, R32(unit, 0xc), R32(unit, 0), game);
    o(10887, np, 7, o(10913, unit) / 3, 0);
    o(10887, np, 6, o(10973, unit, 6, 0) / 3, 0);
    o(10887, np, 0x4a, 0, 0);
    o(10210, np, unit);
    o(10210, unit, np);
    G->sub(0xDFFD0, 0x10, 0, 0, 0, 0, 0, 0, 0);
    R32(counter, 0x18) += 1;
    return 1;
}
