// src/core/d2game/d2game_aea40.cpp -- 1.13c D2Game.dll ItemPropertyTableApplyB (sub_AEA40), transcribed from the cached decompile
// and resolved against its disassembly (the decompile mixes up which locals feed 10680/10945; the assembly is followed here).
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
#define S16(b, o) (*(int16_t*)((uintptr_t)(b) + (o)))
#define U16(b, o) (*(uint16_t*)((uintptr_t)(b) + (o)))

extern "C" __attribute__((weak)) char sgptDataTables_exref[];
static inline uintptr_t DT() { return (uintptr_t)R32(sgptDataTables_exref, 0); }
extern "C" __attribute__((weak)) void D2GAME_9F7A0();   // the stat-list callback handed to 10911
O(10695); O(10428); O(10871); O(10959); O(10973); O(10551); O(11013); O(10785); O(10945); O(10911);
O(10807); O(11043); O(10680); O(10188); O(10913); O(10887);
F(8EA80); F(8E6F0); F(7A260); F(4EA30);

// 1.13c D2Game+0xAEA40 -- ECX = game, EDX = unit, stack = (rec, four more dwords). ret 0x14.
// Applies up to three stat/value pairs of an item property row to the unit; returns 1 when any was applied.
extern "C" S d2_fn_d2game_AEA40(I game, I unit, I rec, I, I, I, I)
{
    S key = rec ? S32(rec, 4) : -1;
    I row = Ord10695(key);
    if (!row) return 0;
    const I extra = R32(row, 0xb0);
    const S n7 = (S)Ord10428(unit, rec, extra);
    const S slot = S16(row, 0x98);
    I node = 0;
    S ofs = 0;
    if (n7 > 0) {
        if (slot < 1 || S32(DT(), 0xc4) <= slot) return 0;
        node = Ord10871(unit, slot);
        if (node) ofs = (S)Ord10959(node) - S32(game, 0xa8);
    }
    S ret = 0;
    bool life = false;
    for (S i = 0; i < 3; i++) {
        const S id = S16(row, 0x9e + i * 2);
        if (id < 0 || S32(DT(), 0xbd4) <= id) break;
        const I srow = (I)((uintptr_t)id * 0x144 + R32(DT(), 0xbcc));
        if (!srow) continue;
        S v = (S)Ord10428(unit, rec, R32(row, 0xa4 + i * 4));
        if (id == 0x1a || id == 0x4a) v <<= 8;
        v <<= (R8(srow, 0x18) & 31);
        if (v <= 0) continue;
        if (id == 6) life = true;
        if ((id == 6 || id == 0x4a) && unit && R32(unit, 0) == 0) v = (S)D2GAME_4EA30(unit, v);
        if (n7 < 1) {
            S cur = (S)Ord10973(unit, id, 0);
            S lim = U16(srow, 0x32);
            if (lim < S32(DT(), 0xbd4)) {
                S l = (S)Ord10973(unit, lim, 0);
                if (cur + v > l) v = l - cur;
            }
            Ord10551(unit, id, v, 0);
        } else {
            const U until = (U)(S32(game, 0xa8) + ofs + n7);
            if (!node) {
                S t = 6, k = -1;
                if (unit) { k = S32(unit, 0xc); t = S32(unit, 0); }
                node = Ord11013(R32(game, 0x1c), 2, until, t, k);
                if (!node) continue;
                Ord10785(node, slot);
                Ord10945(unit, slot, 1);
                if (!unit) {
                    D2GAME_8EA80(game, unit, 3, S32(game, 0xa8) + 1, 0, 0, 0);
                } else if (R32(unit, 0) != 0) {
                    for (I p = (I)R32(unit, 0xdc); p;) {
                        I nx = (I)R32(p, 0x24);
                        if (R8(p, 0) == 3) D2GAME_8E6F0(p, game);
                        p = nx;
                    }
                    D2GAME_8EA80(game, unit, 3, S32(game, 0xa8) + 1, 0, 0, 0);
                }
                Ord10911(node, (I)&D2GAME_9F7A0);
                Ord10807(unit, node, 1);
            }
            Ord11043(node, until);
            S r = (S)Ord10680(node, id, 0);
            S mix = r * ofs + v;
            if (i == 0) D2GAME_8EA80(game, unit, 0xc, until, 0, 0, 0);
            if (ofs + n7 > 0) Ord10188(node, id, mix / (ofs + n7), 0);
        }
        ret = 1;
    }
    if (life && unit && R32(unit, 0) == 0) {
        uint8_t old = (uint8_t)Ord10973(unit, 0x160, 0);
        S a = (S)Ord10973(unit, 6, 0) >> 8;
        S b = (S)Ord10913(unit) >> 8;
        uint8_t nv = (b > 0 && a < b) ? (uint8_t)((a << 7) / b) : 0x80;
        S d = (S)old - (S)nv;
        if (d < 0) d = -d;
        if (d > 4) {
            D2GAME_7A260(unit, nv);
            Ord10887(unit, 0x160, nv, 0);
        }
    }
    return ret;
}
