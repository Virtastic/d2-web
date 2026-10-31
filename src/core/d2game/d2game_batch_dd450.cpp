// src/core/d2game/d2game_batch_dd450.cpp -- 1.13c D2Game.dll attack-result builder (0xDD450), transcribed from the cached
// Ghidra decompile; register/stack inputs recovered from the retail disassembly. Base 0x6FC20000.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameBatchDD450Env {
    void*    (*at)(uint32_t guest);
    int32_t  (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer);
    int32_t  (*o10910)(uint32_t unit, uint32_t stat, uint32_t layer);
    int32_t  (*o10830)(uint32_t unit);
    int32_t  (*o10255)(uint32_t unit);
    int32_t  (*o11104)(uint32_t unit);
    int32_t  (*o10804)(uint32_t unit, uint32_t item, uint32_t a, uint32_t b);
    void     (*sub37540)(uint32_t eaxArg5, uint32_t esiUnit);
    void     (*sub37260)(uint32_t eaxArg5);
    void     (*subDA890)(uint32_t eaxAtt, uint32_t ediTgt);
    void     (*subDA8C0)(uint32_t eaxTgt, uint32_t ebxRec);
    void     (*subDAD70)(uint32_t eaxTgt, uint32_t att, uint32_t rec);
    uint32_t (*subDC530)(uint32_t eaxAdd, uint32_t ecxMax, uint32_t unit, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
    uint32_t (*sub372C0)(uint32_t unit);
    uint32_t (*sub1480)(uint32_t ecxSeed, uint32_t esiMod, uint32_t ediZero);
    uint32_t (*sub11D0)(uint32_t ecxSeed, uint32_t esiOne);
    uint32_t (*sub1180)(uint32_t eaxN, uint32_t ecxSeed);
    int32_t  (*sub14D0)(uint32_t eaxV, uint32_t edxV, uint32_t ecxN);
    // 0xDCD80 : eax = reg operand, then 5 stack args; ediAtt = attacker (edi at the call)
    int32_t  (*subDCD80)(uint32_t eaxV, uint32_t ediAtt, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f);
    uint32_t (*subDBED0)(uint32_t eaxV, uint32_t ecxV, uint32_t edxV, uint32_t unit, uint32_t minV, uint32_t arg3);
    void     (*sub37030)(uint32_t ebxTgt, uint32_t game, uint32_t code, uint32_t att, uint32_t rec);
    void     (*sub770E0)(uint32_t eaxAtt, uint32_t ecxRec, uint32_t tgt);
};
typedef D2GameBatchDD450Env Env_DD;

template <class T> static inline T R(const Env_DD* e, uint32_t a) { T v; memcpy(&v, e->at(a), sizeof(T)); return v; }
template <class T> static inline void W(const Env_DD* e, uint32_t a, T v) { memcpy(e->at(a), &v, sizeof(T)); }

static inline int32_t sc128(int32_t v, uint32_t s) { return (int32_t)((uint32_t)v * s) / 128; }

// 1.13c D2Game+0xDD450 : stack (game, att, tgt, rec, a5, a6), ret 0x18. a6 is a byte (0 -> 0x80).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DD450(uint32_t game, uint32_t att, uint32_t tgt, uint32_t rec, uint32_t a5,
                                                       uint32_t a6, const Env_DD* e) {
    if (!att || !tgt) return;
    uint32_t scale = a6 & 0xff;
    if (scale == 0) scale = 0x80;
    e->sub37540(a5, att);
    uint32_t attType = R<uint32_t>(e, att);
    uint32_t tgtType = R<uint32_t>(e, tgt);
    W<uint32_t>(e, rec, R<uint32_t>(e, rec) | 0x20);
    if ((attType == 0 || e->o10830(att) == 2) && tgtType == 1) {
        e->subDA890(att, tgt);
        if (e->o10255(tgt)) {
            int32_t v = e->o10910(att, 0x79, 0);
            if (v > 0) W<int32_t>(e, rec + 0xc, R<int32_t>(e, rec + 0xc) + v);
        }
        e->subDA8C0(tgt, rec);
        e->subDAD70(tgt, att, rec);
    }
    uint32_t seed = att + 0x20;
    if ((R<uint32_t>(e, rec) & 1) == 0) {
        uint32_t r = e->subDC530(R<uint32_t>(e, rec + 0xc), 0, att, 1, 0, 0, R<uint32_t>(e, rec + 8), scale);
        W<uint32_t>(e, rec + 8, r);
        bool dbl = false;
        if (a5 == 0) {
            uint32_t w = e->sub372C0(att);
            if (w) {
                int32_t v = e->o10804(att, w, 0, 2);
                if (v > 0 && (int32_t)e->sub1480(seed, 0x64, 0) < v) dbl = true;
            }
        }
        if (!dbl) {
            int32_t v = e->o10973(att, 0x151, 0);
            if (v > 0 && (int32_t)e->sub1480(seed, 0x64, 0) < v) dbl = true;
        }
        if (!dbl) {
            int32_t v = e->o10910(att, 0x8d, 0);
            if (v > 0 && (int32_t)e->sub1480(seed, 0x64, 0) < v) dbl = true;
        }
        if (dbl) {
            W<uint8_t>(e, rec + 5, R<uint8_t>(e, rec + 5) | 0x20);
            W<uint32_t>(e, rec + 8, R<uint32_t>(e, rec + 8) << 1);
        }
    }
    // 4 scaled table lookups: (reg, stackA, stackB, 0, 0, previous value) -> rec[4], rec[7], rec[9], rec[8]
    static const struct { uint32_t off, a, b, reg; } T[4] = {
        {0x10, 0x30, 0x149, 0x31}, {0x1c, 0x32, 0x14a, 0x33}, {0x24, 0x36, 0x14b, 0x37}, {0x20, 0x34, 0x165, 0x35}};
    for (int i = 0; i < 4; i++) {
        int32_t v = e->subDCD80(T[i].reg, att, T[i].a, T[i].b, 0, 0, R<uint32_t>(e, rec + T[i].off));
        if (scale != 0x80) v = sc128(v, scale);
        W<int32_t>(e, rec + T[i].off, v);
    }
    int32_t b11104 = e->o11104(att);
    bool addPlain = false;
    if (attType == 1) {
        if (b11104 == 0) {
            static const struct { uint32_t mask, off, a, reg; } M[3] = {{4, 0x38, 0x3c, 0x3d}, {8, 0x3c, 0x3e, 0x3f}, {0x10, 0x40, 0x40, 0x41}};
            for (int i = 0; i < 3; i++) {
                if (R<uint32_t>(e, rec) & M[i].mask) continue;
                int32_t v = e->subDCD80(M[i].reg, att, M[i].a, 0xffffffffu, 0, 0, R<uint32_t>(e, rec + M[i].off));
                W<int32_t>(e, rec + M[i].off, R<int32_t>(e, rec + M[i].off) + (int32_t)((uint32_t)v * scale) / 128);
            }
        } else addPlain = true;
    } else if (attType == 0 || b11104 != 0) {
        addPlain = true;
    }
    if (addPlain) {
        if (!(R<uint32_t>(e, rec) & 4)) W<int32_t>(e, rec + 0x38, R<int32_t>(e, rec + 0x38) + e->o10973(att, 0x3c, 0));
        if (!(R<uint32_t>(e, rec) & 8)) W<int32_t>(e, rec + 0x3c, R<int32_t>(e, rec + 0x3c) + e->o10973(att, 0x3e, 0));
        if (e->o10973(att, 0x67, 0)) W<uint32_t>(e, rec, R<uint32_t>(e, rec) | 0x100);
        if (e->o10973(att, 0x68, 0)) W<uint32_t>(e, rec, R<uint32_t>(e, rec) | 0x200);
        if (e->o10973(att, 0x6a, 0)) W<uint32_t>(e, rec, R<uint32_t>(e, rec) | 0x400);
    }
    int32_t s39 = e->o10973(att, 0x39, 0);
    int32_t s3a = e->o10973(att, 0x3a, 0);
    int32_t s14c = 0;
    if (s3a > 0) s14c = e->o10973(att, 0x14c, 0);
    uint32_t r = e->subDBED0(R<uint32_t>(e, rec + 0x28), (uint32_t)s14c, (uint32_t)s3a, att, (uint32_t)s39, (uint32_t)s14c);
    int32_t ra = sc128((int32_t)r, scale);
    W<int32_t>(e, rec + 0x28, ra);
    if (ra != 0) {
        int32_t v65 = e->o10973(att, 0x65, 0);
        if (v65 > 0) {
            W<int32_t>(e, rec + 0x2c, R<int32_t>(e, rec + 0x2c) + v65);
        } else {
            int32_t cur = R<int32_t>(e, rec + 0x2c) + e->o10973(att, 0x3b, 0);
            W<int32_t>(e, rec + 0x2c, cur);
            int32_t d = e->o10973(att, 0x146, 0);
            if (d > 1) W<int32_t>(e, rec + 0x2c, cur / d);
        }
    }
    if (R<int32_t>(e, rec + 0x24) > 0)
        W<int32_t>(e, rec + 0x30, R<int32_t>(e, rec + 0x30) + sc128(e->o10973(att, 0x38, 0), scale));
    if (R<int32_t>(e, rec + 0x44) == 0)
        W<int32_t>(e, rec + 0x44, R<int32_t>(e, rec + 0x44) + sc128(e->o10973(att, 0x42, 0), scale));
    {
        int32_t base = (int32_t)((uint32_t)R<int32_t>(e, rec + 0x14) * scale) + 0x13c;
        int32_t v = base + (int32_t)e->sub11D0(seed, 1);
        if (v < 0) v = 0;
        W<int32_t>(e, rec + 0x14, R<int32_t>(e, rec + 0x14) + (v >> 7));
    }
    if (R<int32_t>(e, rec + 0x18) != 0)
        W<int32_t>(e, rec + 0x18, R<int32_t>(e, rec + 0x18) + sc128(e->o10973(att, 0x13b, 0), scale));
    if ((attType == 0 || b11104 != 0) && (R<uint32_t>(e, rec + 4) & 0x20) == 0) e->sub37030(tgt, game, 3, att, rec);
    uint8_t kind = R<uint8_t>(e, rec + 0x65);
    if (kind != 0) {
        int32_t d = e->sub14D0(R<uint32_t>(e, rec + 0x68), R<uint32_t>(e, rec + 8), 100);
        int32_t nv = R<int32_t>(e, rec + 8) - d;
        if (nv <= 0) nv = 0;
        W<int32_t>(e, rec + 8, nv);
        if (kind == 0xa) kind = (uint8_t)(e->sub1180(5, seed) + 1);
        auto addv = [&](uint32_t off, int32_t add) { W<int32_t>(e, rec + off, R<int32_t>(e, rec + off) + add); };
        auto atleast = [&](uint32_t off) { if (R<int32_t>(e, rec + off) < 0x32) W<int32_t>(e, rec + off, 0x32); };
        switch (kind) {
        case 1: addv(0x10, d); break;
        case 2: addv(0x1c, d); break;
        case 3: addv(0x20, d); break;
        case 4: addv(0x24, d); atleast(0x30); break;
        case 5: addv(0x28, (d + ((d >> 31) & 7)) >> 3); atleast(0x2c); break;
        case 0xb: addv(0x10, d); atleast(0x18); break;
        case 0xc: addv(0x24, d); atleast(0x34); break;
        default: break;
        }
    }
    e->sub770E0(att, rec, tgt);
    e->sub37260(a5);
}
