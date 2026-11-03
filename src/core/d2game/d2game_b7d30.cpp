// src/core/d2game/d2game_b7d30.cpp -- 1.13c D2Game.dll CreateProjectileFromSkill (sub_B7D30), transcribed from the cached decompile
// resolved against its disassembly (register inputs, out-parameter slots and the x/y roll order are read from the assembly).
// Convention as d2game_batch_aea40.cpp: raw retail records, register inputs explicit, callees weak externs.
#include <cstdint>

typedef intptr_t I;
typedef int32_t S;
typedef uint32_t U;
#define O(n) extern "C" __attribute__((weak)) I Ord##n(...)
#define F(n) extern "C" __attribute__((weak)) I D2GAME_##n(...)
#define R32(b, o) (*(uint32_t*)((uintptr_t)(b) + (o)))
#define S32(b, o) (*(int32_t*)((uintptr_t)(b) + (o)))
#define R8(b, o)  (*(uint8_t*)((uintptr_t)(b) + (o)))
#define R16(b, o) (*(uint16_t*)((uintptr_t)(b) + (o)))

extern "C" __attribute__((weak)) char sgptDataTables_exref[];
static inline uintptr_t DT() { return (uintptr_t)R32(sgptDataTables_exref, 0); }
O(10751); O(10598); O(10867); O(10750); O(10331); O(10913); O(10973); O(10887); O(10210);
F(41490); F(E07A0); F(DFA90); F(21280); F(EF650); F(F20C0); F(20CF0); F(DFFD0);
__attribute__((weak)) __attribute__((import_module("env"), import_name("D2GAME_E03A0"))) I b7d30_E03A0(I, I, I);   // own symbol: other files declare E03A0 with other signatures

// Anchor coordinates: types 2, 4 and 5 read the record at +0x2C directly (+0xC x, +0x10 y); others ask 10867/10750.
static inline S coordX(I u) { U t = R32(u, 0); I p = R32(u, 0x2c); if (t == 2 || (t > 3 && t <= 5)) return S32(p, 0xc); return p ? (S)Ord10867(p) : 0; }
static inline S coordY(I u) { U t = R32(u, 0); I p = R32(u, 0x2c); if (t == 2 || (t > 3 && t <= 5)) return S32(p, 0x10); return p ? (S)Ord10750(p) : 0; }
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
extern "C" uint8_t d2_fn_d2game_B7D30(I unit, I game, I counter)
{
    U slot = (U)unit, flag = 0;
    D2GAME_41490(unit, &slot, &flag);
    if (flag) return 0;
    if (unit && ((R32(unit, 0xc8) >> 10) & 1) && b7d30_E03A0(game, R32(unit, 0x98), R32(unit, 0x94)) != 0)
        if (D2GAME_DFA90() == 0) return 0;
    const S cls = (S32(DT(), 0xa80) > 0x23a) ? 0x23a : 0x23b;
    U outA = 0, outB = 0, outC = 0;
    Ord10751(unit, &outA, &outB, &outC, 1, R8(game, 0x6d), 0);
    D2GAME_E07A0(unit, game);
    I owner = Ord10598(R32(unit, 0x2c));
    I anchor = (owner && owner != unit) ? owner : unit;
    const S x = coordX(anchor), y = coordY(anchor);
    // Two LCG steps on the 64-bit seed at +0x20: v = lo * 0x6AC690C5 + hi; x and y are rolled mod 24 around the anchor.
    uint64_t v1 = (uint64_t)R32(unit, 0x20) * 0x6ac690c5ull + R32(unit, 0x24);
    const S xr = (S)((U)v1 % 0x18) + x - 0xc;
    uint64_t v2 = (uint64_t)(U)v1 * 0x6ac690c5ull + (U)(v1 >> 32);
    *(uint64_t*)((uintptr_t)unit + 0x20) = v2;
    const S yr = (S)((U)v2 % 0x18) + y - 0xc;
    I room = D2GAME_21280(Ord10331(unit), xr, yr);
    if (!room) return 0;
    I np = D2GAME_EF650(cls, 1, game, room, xr, yr, -1, 0);
    if (!np) return 0;
    R32(np, 0xc4) |= 0x4020000;
    D2GAME_F20C0(game, np, 0xf);
    tagLink(unit, R32(unit, 0xc), R32(unit, 0), game);
    D2GAME_20CF0(unit, np, game);
    tagLink(np, R32(unit, 0xc), R32(unit, 0), game);
    Ord10887(np, 7, (S)Ord10913(unit) / 3, 0);
    Ord10887(np, 6, (S)Ord10973(unit, 6, 0) / 3, 0);
    Ord10887(np, 0x4a, 0, 0);
    Ord10210(np, unit);
    Ord10210(unit, np);
    D2GAME_DFFD0(0x10, 0);
    R32(counter, 0x18) += 1;
    return 1;
}
