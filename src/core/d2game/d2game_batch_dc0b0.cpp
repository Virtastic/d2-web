// src/core/d2game/d2game_batch_dc0b0.cpp -- 1.13c D2Game.dll damage-block helpers (0xDC0B0 .. 0xDCE00), transcribed from the
// cached Ghidra decompiles, register and stack inputs recovered from the retail disassembly. Base 0x6FC20000.
// Same conventions as d2game_batch_d9bf0.cpp.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameBatchDC0B0Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);
    int32_t  (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer);
    int32_t  (*o10910)(uint32_t unit, uint32_t stat, uint32_t layer);
    uint32_t (*o10237)(uint32_t diffByte);                     // difficulty table
    int32_t  (*o11104)(uint32_t unit);
    int32_t  (*o10239)(uint32_t unit);
    int32_t  (*o10255)(uint32_t unit);
    int32_t  (*o10051)(uint32_t unit, uint32_t v);
    int32_t  (*o10213)(uint32_t item);                         // returns int16 in ax
    int32_t  (*o10120)(uint32_t item);                         // returns int16 in ax
    int32_t  (*o10804)(uint32_t unit, uint32_t item, uint32_t a, uint32_t b);
    uint32_t (*o10567)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c); // DCE00 min  (args as pushed)
    uint32_t (*o10297)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c); // DCE00 max
    int32_t  (*sub14D0)(uint32_t eaxV, uint32_t edxV, uint32_t ecx400);
    int32_t  (*subDAC10)(uint32_t* ctxEax);                    // returns percent
    void     (*sub37030)(uint32_t ebxC, uint32_t game, uint32_t eleven, uint32_t B, uint32_t rec);
    void     (*subDA7B0)(uint32_t* ctxEsi);
    void     (*subDBD00)(uint32_t entryEax, uint32_t* ctxEcx, uint32_t x);
    uint32_t (*sub372C0)(uint32_t unit);
    uint32_t (*subDBED0)(uint32_t eaxV, uint32_t ecxV, uint32_t edxMax, uint32_t unit, uint32_t minV, uint32_t arg3);
    void     (*sub9F210)(uint32_t eaxIdx, uint32_t a1, uint32_t ebxV);
    uint32_t tableDataTables;                                  // guest address 0x6FD1829C (sgptDataTables slot)
};
typedef D2GameBatchDC0B0Env Env_DC;

template <class T> static inline T R(const Env_DC* e, uint32_t a) { T v; memcpy(&v, e->at(a), sizeof(T)); return v; }
template <class T> static inline void W(const Env_DC* e, uint32_t a, T v) { memcpy(e->at(a), &v, sizeof(T)); }

// pct scaling shared by the unrolled blocks of DC0B0
static int32_t dcScale100(int32_t v, int32_t p) {
    if (v <= 0x100000) {
        if (p <= 0x10000) return (int32_t)((uint32_t)v * (uint32_t)p) / 100;
        return (p / 100) * v;
    }
    return (v / 100) * p;
}

// 1.13c D2Game+0xDC0B0 : retail stack args (game, B, C, rec), ret 0x10.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DC0B0(uint32_t game, uint32_t B, uint32_t C, uint32_t rec, const Env_DC* e) {
    uint32_t ctx[10];
    ctx[0] = game;
    ctx[1] = e->o10237(R<uint8_t>(e, game + 0x6d));
    ctx[2] = B;
    ctx[3] = C;
    ctx[4] = (B && R<uint32_t>(e, B) == 1 && e->o11104(B) == 0) ? 1 : 0;
    ctx[5] = (C && R<uint32_t>(e, C) == 1 && e->o11104(C) == 0) ? 1 : 0;
    ctx[6] = rec;
    ctx[7] = 0;
    int32_t v22 = e->o10973(C, 0x22, 0) << 8;
    if (v22 > 0 && R<int32_t>(e, rec + 0x54) > 0) v22 = e->sub14D0(R<uint32_t>(e, rec + 0x54), (uint32_t)v22, 0x400);
    ctx[8] = (uint32_t)v22;
    int32_t v23 = e->o10973(C, 0x23, 0) << 8;
    if (v23 > 0 && R<int32_t>(e, rec + 0x54) > 0) v23 = e->sub14D0(R<uint32_t>(e, rec + 0x54), (uint32_t)v23, 0x400);
    ctx[9] = (uint32_t)v23;
    int32_t pct = e->subDAC10(ctx);
    if (pct != 100) {
        for (int i = 0; i < 12; i++) {
            uint32_t ent = 0x6fd22ab0u + i * 0x2c;
            if (R<uint32_t>(e, ent + 0x20) == 0) continue;
            uint32_t slot = rec + R<uint32_t>(e, ent);
            int32_t v = R<int32_t>(e, slot);
            if (v <= 0) continue;
            W<int32_t>(e, slot, dcScale100(v, pct));
        }
    }
    e->sub37030(C, game, 0xb, B, rec);
    e->subDA7B0(ctx);
    uint32_t x = 0;
    bool skip = false;
    uint32_t flags = R<uint32_t>(e, rec);
    if (C && R<uint32_t>(e, C) == 1) {
        int32_t p = e->o10239(C), q = e->o10255(C);
        if (p && (flags & 0x100)) x = 1;
        if (q && (flags & 0x200)) x = 1;
        if (p || q) skip = true;
    }
    if (!skip && (flags & 0x400)) x = 1;
    for (int i = 0; i < 12; i++) {
        uint32_t ent = 0x6fd22ab0u + i * 0x2c;
        if (R<uint32_t>(e, ent + 0x1c) != 0 && ctx[4] == 0) break;
        e->subDBD00(ent, ctx, x);
    }
    uint32_t sum = R<uint32_t>(e, rec + 8) + R<uint32_t>(e, rec + 0x10) + R<uint32_t>(e, rec + 0x1c) +
                   R<uint32_t>(e, rec + 0x20) + R<uint32_t>(e, rec + 0x24) + R<uint32_t>(e, rec + 0x28);
    if (ctx[4]) sum += R<uint32_t>(e, rec + 0x38);
    W<uint32_t>(e, rec + 0x4c, sum);
}

// 1.13c D2Game+0xDC530 : eax = base add (edi), ecx = max (ebx), edx = unit (esi); stack arg1 flag, arg2 weapon-or-flag,
// arg3 min, arg4 unused, arg5 scale (low byte), ret 0x14.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_DC530(int32_t eaxAdd, int32_t ecxMax, uint32_t unit, uint32_t a1, uint32_t a2,
                                                           int32_t a3Min, uint32_t a4, uint32_t a5, const Env_DC* e) {
    (void)a4;
    int32_t edi = eaxAdd, ebx = ecxMax, ebp = a3Min;
    uint32_t weapon = a2;
    if (a1) {
        if (!a2) weapon = e->sub372C0(unit);
        if (a2 || weapon) {
            if (e->o10051(unit, R<uint32_t>(e, unit + 0x60)) == 2) {
                ebp = e->o10973(unit, 0x17, 0);
                ebx = e->o10973(unit, 0x18, 0);
            } else {
                ebp = e->o10973(unit, 0x15, 0);
                ebx = e->o10973(unit, 0x16, 0);
            }
        } else {
            ebp = e->o10973(unit, 0x15, 0);
            if (ebp <= 0) ebp = 1;
            ebx = e->o10973(unit, 0x16, 0);
            if (ebx <= 1) ebx = 2;
        }
        ebp <<= 8;
        ebx <<= 8;
    }
    int32_t base = e->o10910(unit, 0x6f, 0) << 8;
    ebp += base;
    ebx += base;
    if (ebp < 1) ebp = 0x100;
    if (ebx <= ebp) ebx = ebp + 0x100;
    edi += e->o10973(unit, 0x19, 0);
    int32_t s12 = e->o10973(unit, 0x12, 0);
    int32_t s11 = e->o10973(unit, 0x11, 0);
    if (weapon) {
        int32_t sb = (int16_t)e->o10213(weapon);
        if (sb) edi += (e->o10973(unit, 0, 0) * sb) / 100;
        int32_t db = (int16_t)e->o10120(weapon);
        if (db) edi += (e->o10973(unit, 2, 0) * db) / 100;
        edi += e->o10804(unit, weapon, 0, 1);
    } else if (a1) {
        edi += e->o10973(unit, 0, 0);
    }
    if (edi <= -0x5a) edi = -0x5a;
    uint32_t r = e->subDBED0((uint32_t)s11, (uint32_t)(s12 + edi), (uint32_t)ebx, unit, (uint32_t)ebp, (uint32_t)(s11 + edi));
    uint32_t sc = a5 & 0xff;
    if (sc == 0x80) return r;
    int32_t v = (int32_t)r;
    if (v <= 0x100000) return (uint32_t)((int32_t)((uint32_t)v * sc) / 128);
    return (uint32_t)((v / 128) * (int32_t)sc);
}

// 1.13c D2Game+0xDCE00 : eax = unit (edi), esi = table index, ebx = pass-through, stack a1 = out record, ret 4.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DCE00(uint32_t eaxUnit, int32_t esiIdx, uint32_t ebxV, uint32_t a1, const Env_DC* e) {
    if (esiIdx < 0) return;
    uint32_t dt = R<uint32_t>(e, R<uint32_t>(e, e->tableDataTables));
    if (esiIdx >= R<int32_t>(e, dt + 0xba0)) return;
    if (R<uint32_t>(e, dt + 0xb98) + (uint32_t)esiIdx * 0x23c == 0) return;
    uint32_t mn = e->o10567(eaxUnit, esiIdx, ebxV, 0);
    uint32_t mx = e->o10297(eaxUnit, esiIdx, ebxV, 0);
    uint32_t r = d2_fn_d2game_DC530(0, (int32_t)mx, eaxUnit, 0, 0, (int32_t)mn, 0, 0x80, e);
    W<uint32_t>(e, a1 + 8, r);
    e->sub9F210((uint32_t)esiIdx, a1, ebxV);
}
