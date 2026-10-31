// src/core/d2game/d2game_batch_dcf20.cpp -- 1.13c D2Game.dll unit-hit dispatch (0xDCF20), transcribed from the cached Ghidra
// decompile; register and stack inputs recovered from the retail disassembly. Base 0x6FC20000.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameBatchDCF20Env {
    void*    (*at)(uint32_t guest);
    uint32_t (*o10331)(uint32_t unit);
    int32_t  (*o10057)(uint32_t v);
    uint32_t (*sub1220)(uint32_t eaxV);
    uint32_t bitMasksSlot;                                     // guest address 0x6FD18BE0 (pointer to the bit-mask table)
    uint32_t (*subDA660)(uint32_t esiRec);
    int32_t  (*o10494)(uint32_t unit, uint32_t v);
    void     (*o10945)(uint32_t unit, uint32_t a, uint32_t b);
    uint32_t (*sub1560)(uint32_t eaxV, uint32_t edxV);
    void     (*sub771D0)(uint32_t ebxRec, uint32_t esiUnit);
    void     (*subDEEE0)(uint32_t ecxA1, uint32_t edxUnit, uint32_t a2, uint32_t one);
    void     (*sub75070)(uint32_t* esiL, uint32_t code);
    void     (*sub75E10)(uint32_t a1, uint32_t* L, uint32_t one);
    void     (*subAF710)(uint32_t eaxUnit, uint32_t code);
    int32_t  (*o10871)(uint32_t unit, uint32_t v);
    int32_t  (*subDB910)(uint32_t unit, uint32_t rec, uint32_t v);
    void     (*sub268E0)(uint32_t ecxA1, uint32_t edxUnit, uint32_t a, uint32_t b);
    void     (*o10158)(uint32_t unit);
    int32_t  (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer);
    uint32_t (*sub77060)(uint32_t eaxV);
    uint32_t (*o10680)(uint32_t a, uint32_t b, uint32_t c);
    uint32_t (*o10630)(uint32_t ecxUnit, uint32_t edxV);
    uint32_t (*o10397)(uint32_t v);
    void     (*o10137)(uint32_t v, uint32_t f);
    void     (*sub78430)(uint32_t eaxA1, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f);
    void     (*sub78550)(uint32_t ecxA1, uint32_t edxUnit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f);
    uint32_t (*sub1420)(uint32_t eaxV);
    void     (*subDFFD0)(uint32_t code, uint32_t unit);
    void     (*o10887)(uint32_t unit, uint32_t stat, int32_t v, uint32_t layer);
};
typedef D2GameBatchDCF20Env Env_DCF;

template <class T> static inline T R(const Env_DCF* e, uint32_t a) { T v; memcpy(&v, e->at(a), sizeof(T)); return v; }
template <class T> static inline void W(const Env_DCF* e, uint32_t a, T v) { memcpy(e->at(a), &v, sizeof(T)); }

// 1.13c D2Game+0xDCF20 : eax = unit; stack a1 (game/room), a2 (point ptr or 0), a3 (hit record); ret 0xc.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_DCF20(uint32_t unit, uint32_t a1, uint32_t a2, uint32_t rec, const Env_DCF* e) {
    uint32_t L[8];
    if (e->o10331(unit) != 0 && e->o10057(e->o10331(unit)) != 0) {
        if (!a2 || R<uint32_t>(e, a2) != 1) return;
        uint32_t r = e->sub1220(R<uint32_t>(e, a2 + 4));
        if (!r) return;
        uint32_t bm = R<uint32_t>(e, e->bitMasksSlot);
        if ((R<uint8_t>(e, bm + 8) & R<uint8_t>(e, r + 0xd)) == 0) return;
    }
    uint32_t flags = R<uint8_t>(e, rec + 4);
    if (flags & 1) {
        if (R<uint8_t>(e, rec + 0x64) == 0 && (R<uint32_t>(e, rec + 0x60) & 0xf0) == 0) W<uint32_t>(e, rec + 0x60, e->subDA660(rec));
        W<uint32_t>(e, unit + 0xb0, R<uint32_t>(e, rec + 0x60));
    }
    if (e->o10494(unit, 0x36) != 0) {
        if (flags & 2) e->o10945(unit, 0x5c, 1);
        return;
    }
    uint32_t type = unit ? R<uint32_t>(e, unit) : 6;
    if (type == 0) {
        uint32_t fl = R<uint16_t>(e, rec + 4);
        if ((fl & 0x80) || (fl & 0x100)) {
            int32_t cond = e->o10871(unit, (fl & 0x80) ? 0x41 : 0x42);
            if (!cond) return;
            uint32_t v = e->o10680((uint32_t)cond, 0x15e, 0);
            uint32_t esi = e->o10630(unit, v);
            if (!esi) return;
            e->o10137(esi, e->o10397(esi) | 4);
            if (!a2) e->sub78430(a1, esi, 0xd, 6, 0xffffffffu, 0);
            else     e->sub78430(a1, esi, 0xd, R<uint32_t>(e, a2), R<uint32_t>(e, a2 + 0xc), 0);
            return;
        }
        if (fl & 0x200) {
            int32_t cond = e->o10871(unit, 0x44);
            if (!cond) return;
            uint32_t v = e->o10680((uint32_t)cond, 0x15e, 0);
            if (!e->o10630(unit, v)) return;
            uint32_t r = e->sub1420(v);
            if (!r) return;
            if ((int16_t)R<uint16_t>(e, r + 0xfc) <= 0) return;
            e->subDFFD0(0xc, unit);
            return;
        }
        if (fl & 0x8010) {
            if (fl & 0x4000) return;
            int32_t s5f = e->o10973(unit, 0x5f, 0);
            int32_t s66 = e->o10973(unit, 0x66, 0);
            int32_t cur = R<int32_t>(e, a1 + 0xa8);
            if (cur - s5f <= ((s66 + ((s66 >> 31) & 7)) >> 3) + 0xf) return;
            e->sub78550(a1, unit, 0, 9, 0, 0, 0);
            e->o10887(unit, 0x5f, R<int32_t>(e, a1 + 0xa8), 0);
            return;
        }
        if (fl & 2) {
            if (!unit) return;
            uint32_t m = R<uint32_t>(e, unit + 0x10);
            if (m == 0 || m == 0x11) return;
            if (!a2) e->sub78430(a1, 0, 0, 6, 0xffffffffu, 0);
            else     e->sub78430(a1, 0, 0, R<uint32_t>(e, a2), R<uint32_t>(e, a2 + 0xc), 0);
            return;
        }
        if (fl & 8) {
            if (!a2) e->sub78430(a1, 0, 0x13, 6, 0xffffffffu, 0);
            else     e->sub78430(a1, 0, 0x13, R<uint32_t>(e, a2), R<uint32_t>(e, a2 + 0xc), 0);
            return;
        }
        if (fl & 4) {
            if (e->o10871(unit, 0x15) != 0 || e->subDB910(unit, rec, R<uint32_t>(e, rec + 0x60)) == 0) {
                e->sub78550(a1, unit, 0, 4, R<uint32_t>(e, rec + 0x4c), 0, 0);
                return;
            }
        } else if (!(fl & 0x4000)) {
            return;
        }
        e->o10158(unit);
        if (unit) W<uint32_t>(e, unit + 0xc4, R<uint32_t>(e, unit + 0xc4) | 0x8000);
        return;
    }
    if (type != 1) return;
    if (!unit) return;
    uint32_t mode = R<uint32_t>(e, unit + 0x10);
    if (mode == 0 || mode == 0xc) return;
    uint32_t fl = R<uint16_t>(e, rec + 4);
    if ((fl & 8) && e->sub1560(R<uint32_t>(e, unit + 4), 0xd) == 0) {
        fl = (fl & 0xfff7) | 4;
        W<uint16_t>(e, rec + 4, (uint16_t)fl);
    }
    e->sub771D0(rec, unit);
    fl = R<uint16_t>(e, rec + 4);
    if (fl & 2) { e->subDEEE0(a1, unit, a2, 1); return; }
    if (fl & 8) {
        e->sub75070(L, 0xd);
        L[2] = a2;
        e->sub75E10(a1, L, 1);
        return;
    }
    if (fl & 0x10) {
        uint32_t cls = R<uint32_t>(e, unit + 4);
        if (!(fl & 0x4000) && cls != 0xf3 && cls != 0x14d && cls != 0x2c1 && e->sub1560(cls, 6) != 0) {
            e->sub75070(L, 6);
            e->sub75E10(a1, L, 1);
            return;
        }
        e->subAF710(unit, 0x13);
        return;
    }
    if (fl & 4) {
        if (e->o10871(unit, 0x15) != 0 || e->subDB910(unit, rec, R<uint32_t>(e, rec + 0x60)) == 0) {
            e->sub75070(L, 3);
            e->sub75E10(a1, L, 1);
            e->sub268E0(a1, unit, 0, 4);
            return;
        }
    } else if (!(fl & 0x4000)) {
        if (!(fl & 1)) return;
        if (R<int32_t>(e, rec + 0x4c) < 1) return;
        uint8_t bl = (uint8_t)e->o10973(unit, 0x160, 0);
        uint8_t al = (uint8_t)e->sub77060((uint32_t)bl);
        int32_t d = (int32_t)bl - (int32_t)al;
        if (d < 0) d = -d;
        if (d < 5) return;
        e->o10158(unit);
        W<uint32_t>(e, unit + 0xc4, R<uint32_t>(e, unit + 0xc4) | 0x8000);
        return;
    }
    e->o10158(unit);
    W<uint32_t>(e, unit + 0xc4, R<uint32_t>(e, unit + 0xc4) | 0x8000);
    e->subAF710(unit, 0x13);
    e->sub268E0(a1, unit, 0, 4);
}
