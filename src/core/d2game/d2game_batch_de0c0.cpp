// src/core/d2game/d2game_batch_de0c0.cpp -- 1.13c D2Game.dll hit application / skill-hit dispatch (0xDE0C0, 0xDE870, 0xDEA00),
// transcribed from the cached Ghidra decompiles; register and stack inputs recovered from the retail disassembly.
// Base 0x6FC20000.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameBatchDE0C0Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);                          // Ordinal_10265(line); Ordinal_10024; _exit(-1)
    uint32_t tableDataTables;                                  // guest address 0x6FD1829C
    uint32_t bitMasksSlot;                                     // guest address 0x6FD18BE0
    uint32_t (*subE0A60)(uint32_t ecxTgt, uint32_t esiAtt, uint32_t game);
    uint32_t (*o10331)(uint32_t unit);
    int32_t  (*o10057)(uint32_t v);
    uint32_t (*sub1220)(uint32_t eaxV);
    uint32_t (*sub15D0)(uint32_t eaxV, uint32_t edxV);
    int32_t  (*subDFA90)(uint32_t eaxAtt);
    void     (*o10009)(uint32_t att, uint32_t tgt);
    void     (*subDC0B0)(uint32_t game, uint32_t tgt, uint32_t att, uint32_t rec);
    void     (*sub37030)(uint32_t ebxUnit, uint32_t game, uint32_t code, uint32_t other, uint32_t rec);
    int32_t  (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer);
    void     (*o10887)(uint32_t unit, uint32_t stat, int32_t v, uint32_t layer);
    void     (*subDBA40)(uint32_t eaxRec, uint32_t ecxAtt, uint32_t edxGame, uint32_t tgt);
    void     (*sub268E0)(uint32_t ecxA1, uint32_t edxUnit, uint32_t a, uint32_t b);
    void     (*subDB570)(uint32_t ediAtt, uint32_t v);
    void     (*subDA970)(uint32_t ediV, uint32_t ebxAtt);
    void     (*subDA930)(uint32_t ediV, uint32_t ebxAtt);
    void     (*subDCC20)(uint32_t eaxTgt, uint32_t ediAtt, uint32_t v);
    void     (*subDC780)(uint32_t eaxTgt, uint32_t v);
    void     (*subDDAC0)(uint32_t eaxAtt, uint32_t ecxTgt, uint32_t v);
    void     (*subDCAB0)(uint32_t tgt, uint32_t att, uint32_t a, uint32_t b);
    void     (*subDC940)(uint32_t tgt, uint32_t att, uint32_t a, uint32_t b);
    void     (*sub8E6F0)(uint32_t eaxP, uint32_t ediGame);
    void     (*sub8EA80)(uint32_t edxAtt, uint32_t ecxGame, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f);
    void     (*sub90F30)(uint32_t att);
    // DE870 / DEA00
    uint32_t (*o10587)(uint32_t unit, uint32_t a, uint32_t b);
    void     (*subDE6D0)(uint32_t eaxUnit, uint32_t ebxReg, uint32_t a1, uint32_t a2, uint32_t a3);
    uint32_t (*sub3A0)(uint32_t eaxV, uint32_t edxV, uint32_t ecxV);   // RVA 0xE03A0
    void     (*subDDCB0)(uint32_t eaxVal, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
    uint32_t (*subDB5E0)(uint32_t ebxUnit, uint32_t game, uint32_t item);
    uint32_t (*o10920)(uint32_t unit);
    int32_t  (*subDC030)(uint32_t eaxN, uint32_t ebxUnit, uint32_t game, uint32_t v);
    int32_t  (*subDFA60)(uint32_t eaxUnit);                            // returns int16
    void     (*subDE8F0)(uint32_t eaxUnit, uint32_t game, uint32_t item, uint32_t v, uint32_t v12, uint32_t n);
};
typedef D2GameBatchDE0C0Env Env_DE;

template <class T> static inline T R(const Env_DE* e, uint32_t a) { T v; memcpy(&v, e->at(a), sizeof(T)); return v; }
template <class T> static inline void W(const Env_DE* e, uint32_t a, T v) { memcpy(e->at(a), &v, sizeof(T)); }

// 1.13c D2Game+0xDE0C0 : eax = attacker (esi); stack (game, tgt, flag, rec), ret 0x10.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DE0C0(uint32_t att, uint32_t game, uint32_t tgt, uint32_t flag, uint32_t rec, const Env_DE* e) {
    uint32_t r = e->subE0A60(tgt, att, game);
    if ((r == 0 && (R<uint32_t>(e, rec) & 0x1000) == 0) || e->o10331(att) == 0) {
        W<uint8_t>(e, rec + 4, R<uint8_t>(e, rec + 4) & 0xfc);
        return;
    }
    if (e->o10057(e->o10331(att)) != 0) {
        bool ok = false;
        if (tgt && R<uint32_t>(e, tgt) == 1) {
            uint32_t q = e->sub1220(R<uint32_t>(e, tgt + 4));
            if (q) ok = (R<uint8_t>(e, R<uint32_t>(e, e->bitMasksSlot) + 8) & R<uint8_t>(e, q + 0xd)) != 0;
        }
        if (!ok) {
            W<uint8_t>(e, rec + 4, R<uint8_t>(e, rec + 4) & 0xfd);
            return;
        }
    }
    uint32_t tgtType = tgt ? R<uint32_t>(e, tgt) : 6;
    uint32_t attType = att ? R<uint32_t>(e, att) : 6;
    if (att && attType == 1 && e->sub15D0(R<uint32_t>(e, att + 4), 0xf) == 0) return;
    if (e->subDFA90(att) != 0) return;
    e->o10009(att, tgt);
    if (flag) e->subDC0B0(game, tgt, att, rec);
    uint32_t w = R<uint16_t>(e, rec + 4);
    if (!(w & 0x20) && (w & 1)) {
        bool doSecond = true;
        if (flag == 0) {
            e->sub37030(tgt, game, 5, att, rec);
            e->sub37030(att, game, 1, tgt, rec);
            doSecond = false;
        } else {
            uint32_t f0 = R<uint32_t>(e, rec);
            if (!(f0 & 0x20)) doSecond = false;
            else {
                if (!(f0 & 0x80)) e->sub37030(tgt, game, 6, att, rec);
                e->sub37030(att, game, 2, tgt, rec);
                doSecond = false;
            }
        }
        (void)doSecond;
    }
    {
        int32_t v = e->o10973(att, 6, 0);
        int32_t cur = R<int32_t>(e, rec + 8);
        W<int32_t>(e, rec + 8, cur < v ? cur : v);
    }
    e->subDBA40(rec, att, game, tgt);
    if (attType == 1 && (R<uint32_t>(e, rec + 4) & 1)) e->sub268E0(game, tgt, 0, 3);
    e->subDB570(att, R<uint32_t>(e, rec + 0x48));
    int32_t d = R<int32_t>(e, rec + 0x4c);
    if (d > 0) {
        int32_t v = e->o10973(att, 6, 0) - d;
        if (v < 0x100) v = 0;
        e->o10887(att, 6, v, 0);
    }
    e->subDA970(R<uint32_t>(e, rec + 0x3c), att);
    e->subDA930(R<uint32_t>(e, rec + 0x40), att);
    e->subDCC20(tgt, att, R<uint32_t>(e, rec + 0x44));
    e->subDC780(tgt, R<uint32_t>(e, rec + 0x30));
    e->subDDAC0(att, tgt, R<uint32_t>(e, rec + 0x34));
    e->subDCAB0(tgt, att, R<uint32_t>(e, rec + 0x28), R<uint32_t>(e, rec + 0x2c));
    e->subDC940(tgt, att, R<uint32_t>(e, rec + 0x14), R<uint32_t>(e, rec + 0x18));
    if (e->o10973(att, 6, 0) <= 0) {
        W<uint8_t>(e, rec + 4, R<uint8_t>(e, rec + 4) | 2);
    } else {
        W<uint8_t>(e, rec + 4, R<uint8_t>(e, rec + 4) & 0xfd);
        if (R<uint32_t>(e, rec + 0x4c) != 0 && attType == 1) {
            if (e->o10973(att, 0x4a, 0) != 0) {
                if (att) {
                    uint32_t p = R<uint32_t>(e, att + 0xdc);
                    while (p) {
                        uint8_t kind = R<uint8_t>(e, p);
                        uint32_t next = R<uint32_t>(e, p + 0x24);
                        if (kind == 3) e->sub8E6F0(p, game);
                        p = next;
                    }
                }
                e->sub8EA80(att, game, 3, R<uint32_t>(e, game + 0xa8) + 1, 0, 0, 0);
            }
            e->sub90F30(att);
        }
    }
    w = R<uint16_t>(e, rec + 4);
    if (!(w & 0x20) && (w & 2)) {
        e->sub37030(att, game, 0xa, tgt, rec);
        e->sub37030(tgt, game, 9, att, rec);
    }
    (void)tgtType;
}

// 1.13c D2Game+0xDE870 : esi = unit, ebx = pass-through (value reg), stack a1; ret 4.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DE870(uint32_t esiUnit, uint32_t ebxV, uint32_t a1, const Env_DE* e) {
    if (!esiUnit) return;
    uint32_t t = R<uint32_t>(e, esiUnit);
    if (t != 0 && t != 1) return;
    uint32_t edi = e->o10587(esiUnit, 0xc, 0);
    if (t == 0) { e->subDE6D0(esiUnit, ebxV, a1, edi, ebxV); return; }
    uint32_t ai = R<uint32_t>(e, esiUnit + 0x14);
    if (!ai) return;
    uint32_t b = R<uint32_t>(e, ai + 0x28);
    if (!b) return;
    uint32_t ecx = R<uint32_t>(e, b + 0x28);
    if (!ecx) return;
    uint32_t p = e->sub3A0(R<uint32_t>(e, b + 0x30), R<uint32_t>(e, b + 0x2c), ecx);
    if (!p || R<uint32_t>(e, p) != 0) return;
    e->subDDCB0(ebxV, a1, p, esiUnit, edi);
}

// 1.13c D2Game+0xDEA00 : stack (game, unit, item), ret 0xc.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DEA00(uint32_t game, uint32_t unit, uint32_t item, const Env_DE* e) {
    if (!unit || !item) return;
    uint32_t t = R<uint32_t>(e, unit);
    if (t != 0 && t != 1) return;
    int32_t n = (int32_t)e->o10587(item, 0xd, 0);
    if (n <= 0) return;
    uint32_t esi = e->subDB5E0(unit, game, item);
    if (!esi) return;
    uint32_t v12 = e->o10587(item, 0xc, 0);
    uint32_t pd = e->o10920(esi);
    if (pd) {
        uint32_t dt = R<uint32_t>(e, R<uint32_t>(e, e->tableDataTables));
        if (R<int32_t>(e, dt + 0xbf0) > 7) {
            uint32_t q = R<uint32_t>(e, pd + 0x44);
            if (q) {
                uint32_t r = R<uint32_t>(e, q) + 0x54;
                uint32_t head = r ? R<uint32_t>(e, r) : 0;
                while (head && (R<uint8_t>(e, head) & 1)) head = R<uint32_t>(e, head + 0x14);
                if (head) {
                    uint32_t cls = R<uint32_t>(e, head + 4);
                    if (game == 0) e->fatal(0x2d3);
                    uint32_t b = R<uint32_t>(e, game + 0x1320 + (cls & 0x7f) * 4);
                    while (b && R<uint32_t>(e, b + 0xc) != cls) b = R<uint32_t>(e, b + 0xe4);
                    if (b) {
                        uint32_t edi = e->o10587(b, 0xc, 0);
                        int32_t val = e->subDC030((uint32_t)n, b, game, v12);
                        if (unit != b) val = (int32_t)((uint32_t)val * 0x56) / 256;
                        e->subDDCB0((uint32_t)val, game, esi, b, edi);
                    }
                }
            }
        }
    }
    uint32_t edi = e->o10587(esi, 0xc, 0);
    int16_t ax = (int16_t)e->subDFA60(esi);
    if (ax != -1) {
        e->subDE8F0(esi, game, item, edi, v12, (uint32_t)n);
        return;
    }
    int32_t res = e->subDC030((uint32_t)n, esi, game, v12);
    e->subDE6D0(esi, esi, game, edi, (uint32_t)res);
}
