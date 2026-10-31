// src/core/d2game/d2game_batch_df620.cpp -- 1.13c D2Game.dll small unit helpers and state transitions
// (0xDF620, 0xE0210, 0xE0660, 0xE0D00, 0xE2310, 0xE2780). Transcribed from the cached Ghidra decompiles;
// register and stack inputs recovered from the retail disassembly. Base 0x6FC20000.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameBatchDF620Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);                        // Ordinal_10265(line); Ordinal_10024; _exit(-1)
    uint32_t bitMasksSlot;                                   // guest address 0x6FD18BE0 (pointer to the mask table)
    uint32_t tableDataTables;                                // guest address 0x6FD1829C
    // DF620
    uint32_t (*o10950)(uint32_t v);
    uint32_t (*o10331)(uint32_t unit);
    uint32_t (*o10826)(uint32_t v);
    uint32_t (*o10459)(uint32_t v);                          // result used as a byte
    uint32_t (*o10717)(uint32_t v);
    void     (*o10451)(uint32_t v);
    // E0210 / E0660
    void     (*sub20D50)(uint32_t eaxUnit, uint32_t* edxId, uint32_t* stackFlag);
    uint32_t (*sub1420)(uint32_t eaxV);
    int32_t  (*subDFA60)(uint32_t eaxUnit);                  // returns int16
    uint32_t (*sub3A0)(uint32_t eaxV, uint32_t edxV, uint32_t ecxV);
    // E0D00
    void     (*o10692)(uint32_t a, uint32_t b);
    void     (*o11003)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f, uint32_t g);
    void     (*sub37E20)(uint32_t ediRec, uint32_t esiGame, uint32_t a, uint32_t b);
    void     (*subAFAB0)(uint32_t eaxRec, uint32_t game, uint32_t a, uint32_t b);
    uint32_t (*subDF9A0)(uint32_t eaxRec);
    void     (*subAF920)(uint32_t eaxGame);
    void     (*subE0860)(uint32_t eaxRec, uint32_t game);
    uint32_t (*o10648)(uint32_t v);
    void     (*o10775)(uint32_t unit, uint32_t v);
    void     (*o10671)(uint32_t unit, uint32_t v);
    void     (*o11056)(uint32_t a, uint32_t b);
    uint32_t (*o10688)(uint32_t v);
    void     (*o10574)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void     (*o10032)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    void     (*o10857)(uint32_t a, uint32_t b);
    uint32_t (*subE0A0)(uint32_t esiRec, uint32_t ediGame);
    void     (*o10158)(uint32_t unit);
    void     (*o10067)(uint32_t unit, uint32_t v);
    // E2310 / E2780
    void     (*sub8E6F0)(uint32_t eaxP, uint32_t ediGame);
    void     (*sub8EA80)(uint32_t edxUnit, uint32_t ecxGame, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f);
    void     (*o10153)(uint32_t a, uint32_t b, uint32_t* out);
    uint32_t (*o10686)(uint32_t ctx, uint32_t x, uint32_t y, uint32_t flags);
};
typedef D2GameBatchDF620Env Env_DF;

template <class T> static inline T R(const Env_DF* e, uint32_t a) { T v; memcpy(&v, e->at(a), sizeof(T)); return v; }
template <class T> static inline void W(const Env_DF* e, uint32_t a, T v) { memcpy(e->at(a), &v, sizeof(T)); }

// 1.13c D2Game+0xDF620 : esi = unit, stack arg1 = game table base, ret 4. Returns the 10950 result or 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_DF620(uint32_t esiUnit, uint32_t tbl, const Env_DF* e) {
    uint32_t edi = R<uint32_t>(e, esiUnit + 0x2c);
    uint32_t ebx = e->o10950(edi);
    uint32_t x = e->o10826(e->o10331(esiUnit));
    if (x) {
        uint32_t idx = e->o10459(x) & 0xff;
        uint32_t p = e->o10717(R<uint32_t>(e, tbl + 0xbc + idx * 4));
        while (p) {
            if (p == ebx) return ebx;
            p = R<uint32_t>(e, p + 0x7c);
        }
    }
    e->o10451(edi);
    return 0;
}

// 1.13c D2Game+0xE0210 : eax = unitA, esi = unitB, ecx = row index. Returns 1/0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E0210(uint32_t eaxA, uint32_t esiB, int32_t ecxIdx, const Env_DF* e) {
    if (!esiB || !eaxA) return 0;
    if (R<uint32_t>(e, esiB) != 0 && R<uint32_t>(e, eaxA) != 1) return 0;
    uint32_t dt = R<uint32_t>(e, R<uint32_t>(e, e->tableDataTables));
    if (ecxIdx < 0 || ecxIdx >= R<int32_t>(e, dt + 0xba0)) return 0;
    uint32_t row = R<uint32_t>(e, dt + 0xb98) + (uint32_t)ecxIdx * 0x23c;
    if (!row) return 0;
    if ((R<uint8_t>(e, R<uint32_t>(e, e->bitMasksSlot) + 4) & R<uint8_t>(e, row + 7)) == 0) return 0;
    uint32_t flag = 0, id = 0;
    e->sub20D50(eaxA, &id, &flag);
    return (flag == 0 && id == R<uint32_t>(e, esiB + 0xc)) ? 1 : 0;
}

// 1.13c D2Game+0xE0660 : eax = unit, stack (a1, player, a3), ret 0xc. Returns 1/0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E0660(uint32_t eaxUnit, uint32_t a1, uint32_t player, uint32_t a3, const Env_DF* e) {
    if (!player || !eaxUnit || R<uint32_t>(e, player) != 0) return 0;
    uint32_t type = R<uint32_t>(e, eaxUnit);
    if (type != 0 && type != 1) return 0;
    uint32_t row = e->sub1420(a3);
    if (!row) return 0;
    if ((R<uint8_t>(e, R<uint32_t>(e, e->bitMasksSlot) + 8) & R<uint8_t>(e, row + 7)) == 0) return 0;
    uint32_t id = R<uint32_t>(e, eaxUnit + 0xc);
    uint32_t flag = type;
    if (type == 1) {
        e->sub20D50(eaxUnit, &id, &flag);
        if (id == 0xffffffffu || flag != 0) return 0;
    }
    int16_t s1 = (int16_t)e->subDFA60(player);
    if (s1 == -1) return 0;
    uint32_t r = e->sub3A0(flag, id, a1);
    if (!r) return 0;
    int16_t s2 = (int16_t)e->subDFA60(r);
    return s1 == s2 ? 1 : 0;
}

// 1.13c D2Game+0xE0D00 : eax = edi reg, ecx = unit (esi), edx = rec (ebx), stack (game, a2), ret 8.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E0D00(uint32_t eaxEdi, uint32_t ecxUnit, uint32_t edxRec, uint32_t game, uint32_t a2, const Env_DF* e) {
    const uint32_t edi = eaxEdi, esi = ecxUnit, ebx = edxRec;
    if (!game) { e->fatal(0x45d); return; }
    if (!ebx)  { e->fatal(0x45e); return; }
    uint8_t al = (uint8_t)e->o10459(e->o10826(esi));
    W<uint8_t>(e, ebx + 0x18, al);
    W<uint32_t>(e, ebx + 0x1c, R<uint32_t>(e, game + 0xbc + (uint32_t)al * 4));
    W<uint32_t>(e, ebx + 0xc8, R<uint32_t>(e, ebx + 0xc8) & 0xfffffeffu);
    auto release = [&]() {
        e->o10692(R<uint32_t>(e, game + 0x1c), R<uint32_t>(e, ebx + 0x2c));
        W<uint32_t>(e, ebx + 0x2c, 0);
        e->o11003(R<uint32_t>(e, game + 0x1c), esi, a2, edi, ebx, 0);
    };
    bool toE11 = false;
    switch (R<uint32_t>(e, ebx)) {
    case 0:
        release();
        e->sub37E20(ebx, game, a2, edi);
        {
            uint32_t s = R<uint32_t>(e, ebx + 0x10);
            toE11 = (s == 0x11 || s == 0);
        }
        break;
    case 1:
        release();
        e->subAFAB0(ebx, game, a2, edi);
        if (e->subDF9A0(ebx)) e->subAF920(game); else e->subE0860(ebx, game);
        toE11 = e->o10648(ebx) != 0;
        break;
    case 2: {
        uint32_t k = e->o10688(R<uint32_t>(e, ebx + 4));
        e->o10574(ebx, esi, a2, edi);
        if (R<uint8_t>(e, R<uint32_t>(e, ebx + 0x10) + k + 0x120) != 0) e->o10032(ebx, esi, a2, edi);
        e->o10857(ebx, esi);
        break;
    }
    case 3:
        release();
        break;
    case 4:
        if (R<uint32_t>(e, ebx + 0x10) == 3) {
            e->o10574(ebx, esi, a2, edi);
            e->o10032(ebx, esi, a2, edi);
            e->o10857(ebx, esi);
            uint32_t r = e->subE0A0(ebx, game);
            W<uint32_t>(e, R<uint32_t>(e, ebx + 0x14) + 0x24, r);
        }
        break;
    case 5:
        e->o10574(ebx, esi, a2, edi);
        e->o10032(ebx, esi, a2, edi);
        e->o10857(ebx, esi);
        break;
    default:
        e->fatal(0x4ab);
        return;
    }
    if (toE11) {
        e->o10775(ebx, 1);
        e->o10671(ebx, 5);
        e->o11056(R<uint32_t>(e, ebx + 0x2c), 0x8000);
    }
    if (e->o10331(ebx) != 0) e->o10158(ebx);
    e->o10067(ebx, 1);
}

// 1.13c D2Game+0xE2310 : ebx = unit, stack (game, d2), ret 8.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E2310(uint32_t ebxUnit, uint32_t game, int32_t d2, const Env_DF* e) {
    if (ebxUnit) {
        for (int pass = 0; pass < 2; ++pass) {
            uint32_t p = R<uint32_t>(e, ebxUnit + 0xdc);
            while (p) {
                uint8_t k = R<uint8_t>(e, p);
                uint32_t next = R<uint32_t>(e, p + 0x24);
                if (k == (uint8_t)pass) e->sub8E6F0(p, game);
                p = next;
            }
        }
    }
    int32_t edi = R<int32_t>(e, game + 0xa8);
    W<int32_t>(e, ebxUnit + 0x44, (int32_t)((uint32_t)(edi - d2) << 8));
    int32_t iVar6, iVar5;
    if (R<uint32_t>(e, ebxUnit + 0x30) == 0) {
        iVar6 = (int32_t)R<int16_t>(e, ebxUnit + 0x4c);
        iVar5 = R<int32_t>(e, ebxUnit + 0x48);
    } else {
        iVar6 = R<int32_t>(e, ebxUnit + 0x3c);
        iVar5 = R<int32_t>(e, ebxUnit + 0x34);
    }
    if (iVar6 == 0) {
        e->sub8EA80(ebxUnit, game, 1, (uint32_t)(R<int32_t>(e, game + 0xa8) + 1), 0, 0, 0);
        return;
    }
    uint32_t c = R<uint32_t>(e, ebxUnit + 0x50);
    int32_t esi = d2 - 1;
    int32_t ecx = (int32_t)((uint32_t)d2 << 8);
    int32_t eax = ecx + iVar6;
    while (eax < iVar5) {
        int32_t local4 = ecx + iVar6;
        int32_t local8 = eax + iVar6;
        int32_t bound = local4 >> 8;
        edi += 1;
        for (; esi <= bound; ++esi) {
            uint32_t v;
            if (R<uint32_t>(e, ebxUnit + 0x30) == 0) v = R<uint8_t>(e, (uint32_t)esi + c + 0x10);
            else e->o10153(R<uint32_t>(e, ebxUnit + 0x30), (uint32_t)esi << 8, &v);
            if ((int32_t)v > 0 && (int32_t)v <= 4)
                e->sub8EA80(ebxUnit, game, 0, (uint32_t)edi, 0, v, 0);
        }
        ecx = local4;
        eax = local8;
    }
    if ((uint32_t)edi == R<uint32_t>(e, game + 0xa8)) edi += 1;
    e->sub8EA80(ebxUnit, game, 1, (uint32_t)(edi + 1), 0, 0, 0);
}

// 1.13c D2Game+0xE2780 : eax = game, esi = out (x, y), ebx = ctx for the 10686 probe. Returns 1/0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E2780(uint32_t eaxGame, uint32_t esiOut, uint32_t ebxCtx, const Env_DF* e) {
    uint32_t list = R<uint32_t>(e, eaxGame + 0x10f4);
    if (!list) return 0;
    uint32_t n = R<uint32_t>(e, list);
    while (n && R<uint32_t>(e, n) != 0x1c) n = R<uint32_t>(e, n + 0xf4);
    if (!n) return 0;
    uint32_t d = R<uint32_t>(e, n + 0x18);
    if (R<uint8_t>(e, d) == 0) return 0;
    int32_t x = R<int32_t>(e, d + 4), y = R<int32_t>(e, d + 8);
    W<int32_t>(e, esiOut, x);
    W<int32_t>(e, esiOut + 4, y);
    if (R<uint8_t>(e, d + 2) != 0) return 1;
    auto probe = [&]() { return e->o10686(ebxCtx, (uint32_t)R<int32_t>(e, esiOut), (uint32_t)R<int32_t>(e, esiOut + 4), 0x3c01) != 0; };
    auto commit = [&]() {
        W<int32_t>(e, d + 4, R<int32_t>(e, esiOut));
        W<int32_t>(e, d + 8, R<int32_t>(e, esiOut + 4));
    };
    auto bump = [&](int dx, int dy) {
        W<int32_t>(e, esiOut, R<int32_t>(e, esiOut) + dx);
        W<int32_t>(e, esiOut + 4, R<int32_t>(e, esiOut + 4) + dy);
    };
    bump(0, -0x14);
    W<uint8_t>(e, d + 2, 1);
    if (probe()) {
        bump(0, 9);
        if (probe()) {
            bump(2, 0);
            if (probe()) {
                bump(-4, 0);
                if (probe()) {
                    W<int32_t>(e, esiOut, R<int32_t>(e, d + 4));
                    W<int32_t>(e, esiOut + 4, R<int32_t>(e, d + 8) - 8);
                    if (probe()) {
                        bump(0, 5);
                        commit();
                        return 1;
                    }
                    commit();
                    return 1;
                }
            }
        }
    }
    commit();
    return 1;
}
