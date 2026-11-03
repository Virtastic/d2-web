// 1.13c D2Game+0x80B00 spider-type (Arach) alerted think step (retail layout). ECX = P (the AI parameter owner: byte +0x6D
// indexes the 16-bit parameter tables of the record below), EDX = T (target unit), stack word 0 = a (ret 4): a[0] control block
// (mode +0x14, last +0x18, counter +0x1C), a[8] passed through, a[0x14] a stored count, a[0x18] the alerted flag, a[0x1C] the
// parameter record (s16 tables at +0x56 +0x5C +0x62 +0x68 +0x6E, word +0x170 and byte +0x180). Seeds: T +0x20 (lo) / +0x24 (hi),
// seed = lo * 0x6AC690C5 + hi, roll = lo % 100; the inline rolls advance it here, the 0x1180 calls advance it in the callee.
// Every callee rides in Env so a probe records it.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2G80B00Env {
    int32_t  (*dist)(uint32_t t);                                                    // 0xA718 (one stack word)
    int32_t  (*roll)(uint32_t seed);                                                 // 0x1180 (EAX 100, ECX seed pointer)
    void     (*f2220)(uint32_t p, uint32_t t, uint32_t a8);                          // 0xF2220 (EAX 4, EDI T; P, a8)
    void     (*f26b0)(uint32_t p, uint32_t t, uint32_t a8, uint32_t imm);            // 0xF26B0 (EAX 0, ECX T; P, a8, imm)
    void     (*f2d80)(uint32_t p, uint32_t t, uint32_t imm, uint32_t a8);            // 0xF2D80 (EAX a8; P, T, imm, 0)
    void     (*f2380)(uint32_t t, uint32_t rec, uint32_t w, uint32_t b);             // 0xF2380 (ECX T, EAX rec|byte; w, 0, 0, 0)
    void     (*f1b10)(uint32_t p, uint32_t t, int32_t rollv);                        // 0xF1B10 (EAX rollv with AL 6; P, T)
    void     (*f22c0)(uint32_t p);                                                   // 0xF22C0 (EAX P; 0xF)
    void     (*f3000)(uint32_t a8, uint32_t p, uint32_t t);                          // 0xF3000 (ECX a8, EDX T; P, 0)
    void     (*f2540)(uint32_t p, uint32_t t, uint32_t a8);                          // 0xF2540 (ECX P, EDX T; a8, 2, 0, 0, 1, 0)
    int32_t  (*b0be0)(uint32_t t);                                                   // 0xB0BE0 (EAX T)
    void     (*a496)(uint32_t t);                                                    // 0xA496 (T, 0)
    int32_t  (*a68e)(uint32_t t);                                                    // 0xA68E (T, 0x16)
    uint32_t (*rec1320)(uint32_t t4);                                                // 0x1320 (EAX [T+4])
    void     (*e75e10)(uint32_t p, const uint32_t* blk);                             // 0x75E10 (P, &block, 1)
    const uint32_t* bitmask;                                                         // the module global 0xF8BE0 (a pointer to a table; +0x10)
};

static inline uint32_t rd_80b00(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr_80b00(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static inline int32_t s16_80b00(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
static inline uint32_t lcg_80b00(uint32_t seed) {
    uint32_t lo = rd_80b00(seed), hi = rd_80b00(seed + 4);
    const uint64_t s = (uint64_t)lo * 0x6AC690C5u + hi;
    std::memcpy((void*)(uintptr_t)seed, &s, 8);
    return (uint32_t)s;
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_80B00(uint32_t P, uint32_t T, uint32_t a, const D2G80B00Env* e)
{
    const uint32_t ctl = rd_80b00(a), a14 = rd_80b00(a + 0x14), flag = rd_80b00(a + 0x18), tbl = rd_80b00(a + 0x1C), a8 = rd_80b00(a + 8);
    const uint32_t idx = *(const uint8_t*)(uintptr_t)(P + 0x6D) * 2, seed = T + 0x20;
    const int32_t d = e->dist(T);
    if (rd_80b00(ctl + 0x14) == 1) {
        wr_80b00(ctl + 0x18, 0);
        if (d > 0x4B) {
            wr_80b00(ctl + 0x14, 0);
            if ((int32_t)(lcg_80b00(seed) % 100u) < s16_80b00(tbl + 0x62 + idx)) { wr_80b00(ctl + 0x14, 2); e->f3000(a8, P, T); return; }
            int32_t m = (lcg_80b00(seed) & 0xFF) < 0x80 ? 5 : 6;
            const uint32_t u = (T && rd_80b00(T) == 1 && rd_80b00(T + 0x14)) ? rd_80b00(rd_80b00(T + 0x14) + 0x2C) : 0;
            if (m) wr_80b00(u + 0x18, (uint32_t)m);
            *(uint8_t*)(uintptr_t)(u + 0x20) = 6;
            e->f2540(P, T, a8);
            return;
        }
        if (flag && s16_80b00(tbl + 0x56 + idx) > 0x19 && e->roll(seed) < s16_80b00(tbl + 0x56 + idx) - 0x19) { e->f2220(P, T, a8); return; }
        if ((int32_t)a14 >= s16_80b00(tbl + 0x68 + idx) && !e->b0be0(T)) { wr_80b00(ctl + 0x14, 0); e->f26b0(P, T, a8, 0xC); return; }
        e->f2d80(P, T, 4, a8);
        return;
    }
    if (flag) {
        wr_80b00(ctl + 0x14, 2);
        const int32_t r = (int32_t)(lcg_80b00(seed) % 100u);
        if (r < s16_80b00(tbl + 0x56 + idx)) {
            e->a496(T);
            const uint32_t rc = T ? e->rec1320(rd_80b00(T + 4)) : 0;
            uint32_t sel = 100;
            if (rc && (*(const uint8_t*)(uintptr_t)(rc + 0x104) & rd_80b00(rd_80b00((uint32_t)(uintptr_t)e->bitmask) + 0x10))) sel = 0x65;
            const uint32_t blk[8] = { 4, T, a8, 0, 0, sel << 8, 0, 0 };
            e->e75e10(P, blk);
            return;
        }
        if (d < s16_80b00(tbl + 0x6E + idx)) {
            wr_80b00(ctl + 0x14, 1);
            if (s16_80b00(tbl + 0x170) >= 0 && !e->a68e(T)) { e->f2380(T, tbl, (uint32_t)s16_80b00(tbl + 0x170), *(const uint8_t*)(uintptr_t)(tbl + 0x180)); return; }
            e->f2d80(P, T, 8, a8);
            return;
        }
        if (e->roll(seed) < s16_80b00(tbl + 0x5C + idx)) { e->f26b0(P, T, a8, 4); return; }
        e->f22c0(P);
        return;
    }
    if (!((T && rd_80b00(T) == 1 && rd_80b00(T + 0x14) && (rd_80b00(rd_80b00(T + 0x14) + 0x54) == 3 || rd_80b00(rd_80b00(T + 0x14) + 0x54) == 0x13))) &&
        rd_80b00(ctl + 0x18) != 1) {
        uint32_t c = rd_80b00(ctl + 0x1C) + 1;
        wr_80b00(ctl + 0x1C, c);
        if ((int32_t)c > 0x14) wr_80b00(ctl + 0x1C, 0);
        wr_80b00(ctl + 0x18, 0);
        if (rd_80b00(ctl + 0x1C) == 1 && e->roll(seed) < s16_80b00(tbl + 0x62 + idx)) { wr_80b00(ctl + 0x18, 1); e->f3000(a8, P, T); return; }
        const int32_t rv = e->roll(seed);
        if (rv < 0x14) { e->f1b10(P, T, rv); return; }
        e->f22c0(P);
        return;
    }
    wr_80b00(ctl + 0x18, 1);
    e->f3000(a8, P, T);
}
