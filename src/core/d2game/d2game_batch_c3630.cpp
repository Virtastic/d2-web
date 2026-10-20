// src/core/d2game/d2game_batch_c3630.cpp -- twelve 1.13c D2Game.dll functions (0xC3630..0xC5100)
// transcribed from the cached Ghidra decompiles. Retail addresses arrive as 32-bit integers; callees
// not implemented here are seams on D2GameBatchC3630Env in retail argument order. Register-only
// arguments (in_EAX / unaff_EDI / unaff_ESI / unaff_retaddr) are explicit parameters, named per function.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <emscripten.h>

static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline void wr8 (uint32_t a, uint32_t o, uint8_t v)  { *(uint8_t*)(uintptr_t)(a + o) = v; }
static inline void wr16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 2); }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }

struct D2GameBatchC3630Env {
    uint32_t (*sub2E660)(uint32_t tag);                       // 0xE660 (tag = fourcc, e.g. 'hst ')
    uint32_t (*ordinal10920)(uint32_t ptr);
    int32_t  (*ordinal10174)(uint32_t a, uint32_t b, uint32_t c);
    int32_t  (*ordinal10174Reg)();                            // call with no stack args (0xC3BD0)
    void     (*ordinal10593)(uint32_t a, uint32_t b, uint32_t c);
    void     (*ordinal10003)();
    uint32_t (*ordinal10265)(uint32_t size);
    void     (*ordinal10024)(const char* file, uint32_t value);
    const char* assertFile;                                   // DAT_6FD19FF9
    void     (*sub540C0)();                                   // 0x340C0
    void     (*sub53830)(uint32_t p);                         // 0x33830
    uint32_t (*sub53800)(uint32_t ecx, uint32_t p);           // 0x33800
    void     (*sub53EB0)(uint32_t self, uint32_t a, uint32_t id); // 0x33EB0
    uint32_t (*sub538D0)(uint32_t a, uint32_t b);             // 0x338D0
    void     (*sub81420)(uint32_t ecx, uint32_t p);           // 0x61420
    void     (*sub21180)(uint32_t seedPtr);                   // 0x1180
    void     (*sub554F0)(uint32_t self, uint32_t unit);       // 0x354F0
    uint32_t (*sub2120)();                                    // 0xC2120
    void     (*sub8A3E0)(void* buf, uint32_t len);            // 0x8A3E0
};

static uint32_t cur(const D2GameBatchC3630Env* e, uint32_t unit, uint32_t game)
{   // *(game_rec + 0x10 + unit[0x6d]*4)
    uint32_t r = e->ordinal10920(game);
    return rd32(r, 0x10 + (uint32_t)rd8(unit, 0x6d) * 4);
}
// Walk the unit's +0x10f4 list (head deref) for node kind `kind`; next link at node+0xF4.
static uint32_t findKind(uint32_t head, int32_t kind)
{
    if (!head) return 0;
    uint32_t n = rd32(head, 0);
    while (n && (int32_t)rd32(n, 0) != kind) n = rd32(n, 0xF4);
    return n;
}

// 1.13c D2Game+0xC3630
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_C3630(uint32_t p1, uint32_t p2, const D2GameBatchC3630Env* e)
{
    if (e->sub2E660(0x20706976) == 0 && e->sub2E660(0x20747368) == 0) {
        uint32_t u = cur(e, p1, p2);
        if (e->ordinal10174(u, 10, 0) == 0 && rd32(p1, 0x10f4) != 0) {
            uint32_t n = findKind(rd32(p1, 0x10f4), 10);
            if (!n) return 0;
            uint32_t s = rd32(n, 0x18);
            wr32(s, 0x9c, rd32(s, 0x9c) + 1);
        }
    }
    return 0;
}

// 1.13c D2Game+0xC3970
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_C3970(uint32_t p1, uint32_t p2, const D2GameBatchC3630Env* e)
{
    e->sub540C0();
    (void)p2;   // both arms of the *(p2+0xC)==0 test call the same function
    e->sub53830(rd32(p1, 0x18) + 0x14);
}

// 1.13c D2Game+0xC3A80   unaff_ESI / unaff_EDI explicit
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_C3A80(uint32_t p1, uint32_t p2, uint32_t esi, uint32_t edi, const D2GameBatchC3630Env* e)
{
    uint32_t unit = rd32(p2, 0);
    uint32_t u = cur(e, unit, rd32(p2, 0xC));
    uint32_t msf = e->sub2E660(0x2066736d), box = e->sub2E660(0x20786f62);
    uint32_t vip = e->sub2E660(0x20706976), hst = e->sub2E660(0x20747368);
    uint32_t s = rd32(p1, 0x18);
    if (msf) wr32(s, 0x18, rd32(s, 0x18) + 1);
    if (vip) wr32(s, 0x24, rd32(s, 0x24) + 1);
    if (box) wr32(s, 0x20, rd32(s, 0x20) + 1);
    if (hst) wr32(s, 0x1c, rd32(s, 0x1c) + 1);
    if (e->ordinal10174(u, 10, 0) != 0) return;
    if (rd32(unit, 0x10f4) == 0) return;
    uint32_t n = findKind(rd32(unit, 0x10f4), 0xd);
    if (!n) return;
    if (rd8(n, 9) == 0) return;
    if (rd8(rd32(n, 0x18), 0xb) != 0) return;
    if (e->sub2E660(0x20747368) != 0) return;
    if (msf == 0) {
        e->ordinal10593(u, 10, 5);
        if (edi == 0) goto six;
    } else if (edi != 0) {
        if (esi != 0) return;
        if (edi == 0) goto six;
    } else {
    six:
        e->ordinal10593(u, 10, 6);
    }
    if (esi == 0) e->ordinal10593(u, 10, 4);
}

// 1.13c D2Game+0xC3BD0   thiscall; unaff_retaddr -> outA, unaff_EDI -> outB, puStack_c -> outC (byte stores)
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_C3BD0(uint32_t self, uint32_t p1, uint32_t p2, uint32_t outA, uint32_t outB,
                            uint32_t outC, const D2GameBatchC3630Env* e)
{
    (void)p1;
    if (e->ordinal10174(p2, 7, 0) == 0) { wr8(outA, 0, 0); return 1; }
    if (e->ordinal10174Reg() != 0 || e->ordinal10174(p2, 10, 1) != 0) {
        if (e->ordinal10174(p2, 0xe, 0) == 0) {
            wr8(p2, 0, e->ordinal10174(p2, 10, 10) != 0 ? 5 : 6);
            return 1;
        }
        wr8(p2, 0, (e->ordinal10174(p2, 10, 0xd) != 0 ? 2 : 0) + 0xb);
        return 1;
    }
    wr8(outC, 0, 0);
    uint32_t tr1 = e->sub2E660(0x20317274);
    uint32_t msf = e->sub2E660(0x2066736d);   // used as a store address by the retail code
    e->sub2E660(0x20786f62);
    e->sub2E660(0x20706976);
    uint32_t hst = e->sub2E660(0x20747368);
    int32_t r3 = e->ordinal10174(p2, 10, 3);
    if (p2 != 0 && tr1 != 0) {
        if (e->ordinal10174(p2, 10, 6) == 0) { wr8(msf, 0, (r3 == 0) * 2 + 2); return 1; }
        wr8(msf, 0, 3);
        return 1;
    }
    if (hst != 0) {
        wr8(msf, 0, e->ordinal10174(p2, 10, 10) != 0 ? 5 : 6);
        return 1;
    }
    uint32_t s = rd32(self, 0x18);
    if ((rd8(s, 0x29) != 0 && rd32(s, 0x20) == 0) ||
        (rd32(s, 0x24) == 0 && (uint8_t)e->sub2120() != 0) ||
        (rd8(s, 0x28) != 0 && rd32(s, 0x18) == 0))
        wr8(outB, 0, 9);
    wr8(outB, 0, r3 != 0 ? 2 : 4);
    return 1;
}

// 1.13c D2Game+0xC4170   unit arrives on the stack slot Ghidra calls unaff_retaddr; ecx for 10920
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_C4170(uint32_t unit, uint32_t ecx, const D2GameBatchC3630Env* e)
{
    uint32_t u = cur(e, unit, ecx);
    if (e->ordinal10174(u, 10, 0) == 0 && e->ordinal10174(u, 10, 0xf) == 0) return 1;
    if (e->ordinal10174(u, 10, 0xd) == 0 && e->ordinal10174(u, 10, 0xe) == 0) return 0;
    return 1;
}

// 1.13c D2Game+0xC4570
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_C4570(uint32_t self, uint32_t p2, const D2GameBatchC3630Env* e)
{
    int32_t id = rd32(p2, 8) == 0 ? -1 : (int32_t)rd32(rd32(p2, 8), 4);
    uint32_t u = cur(e, rd32(p2, 0), rd32(p2, 0xC));
    if (id == 0xc9) {
        if (e->ordinal10174(u, 8, 0) == 0) e->sub53EB0(self, rd32(p2, 0x14), 0xc9);
    } else if (id == 0xf4) {
        uint32_t s = rd32(self, 0x18);
        if (e->ordinal10174(u, 4, 0xe) == 1 && e->sub53800(self, s) == 0) {
            e->sub53EB0(self, rd32(p2, 0x14), 0xf4);
            e->sub81420(self, rd32(p2, 0xC));
        }
    }
}

// 1.13c D2Game+0xC4900
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_C4900(uint32_t self, uint32_t p2, const D2GameBatchC3630Env* e)
{
    if (rd32(p2, 8) == 0 || rd32(rd32(p2, 8), 4) != 0x179) return;
    uint32_t u = cur(e, rd32(p2, 0), rd32(p2, 0xC));
    if (e->ordinal10174(u, 0x1e, 0) == 0 && e->ordinal10174(u, 9, 0) == 0 &&
        e->ordinal10174(u, 9, 0xd) == 0 && e->ordinal10174(u, 9, 1) == 0 &&
        e->sub538D0(9, 0xd) == 0)
    {
        uint32_t head = rd32(rd32(self, 4), 0x10f4);
        if (head) {
            for (uint32_t n = rd32(head, 0); n; n = rd32(n, 0xF4)) {
                if (rd32(n, 0) == 8) {
                    if (rd8(n, 9) == 0) {
                        e->sub21180(rd32(p2, 0xC) + 0x20);
                        e->sub53EB0(self, rd32(p2, 0x14), 0x179);
                        return;
                    }
                    break;
                }
            }
        }
        if (e->ordinal10174(u, 0x1e, 0xd) == 0) {
            e->sub21180(rd32(p2, 0xC) + 0x20);
            e->sub53EB0(self, rd32(p2, 0x14), 0x179);
            return;
        }
    }
    uint32_t g = rd32(p2, 0xC);
    uint64_t seed = (uint64_t)rd32(g, 0x20) * 0x6ac690c5ull + rd32(g, 0x24);
    std::memcpy((void*)(uintptr_t)(g + 0x20), &seed, 8);
    e->sub53EB0(self, rd32(p2, 0x14), 0x179);
}

// 1.13c D2Game+0xC4C30   in_EAX = eax (unit)
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_C4C30(uint32_t p1, uint32_t eax, const D2GameBatchC3630Env* e)
{
    e->ordinal10003();
    e->ordinal10003();
    uint32_t r = 0;
    if (p1 != 0 && rd32(p1, 0) == 0) r = rd32(e->ordinal10920(p1), 0x9c);
    uint32_t w = rd16(r, 10);
    uint32_t v = ((w & 0x20) | 0x80) >> 5;
    if (((w & 0x1f00) >> 8) <= v * rd8(eax, 0x6d) + 2)
        wr16(r, 10, (uint16_t)(((uint16_t)v * rd8(eax, 0x6d) + 2) * 0x100 | (w & 0xe0ff)));
}

static uint32_t c4d30(const D2GameBatchC3630Env* e, uint32_t unit, uint32_t g, uint32_t cat)
{
    uint32_t u = cur(e, unit, g);
    if ((e->ordinal10174(u, cat, 0) != 0 || e->ordinal10174(u, cat, 0xf) != 0) &&
        e->ordinal10174(u, cat, 0xd) == 0 && e->ordinal10174(u, cat, 0xe) == 0)
        return 0;
    e->sub554F0(g, unit);
    return 0;
}
// 1.13c D2Game+0xC4D30
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_C4D30(uint32_t p1, uint32_t p2, const D2GameBatchC3630Env* e) { return c4d30(e, p1, p2, 0xe); }
// 1.13c D2Game+0xC4E30
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_C4E30(uint32_t p1, uint32_t p2, const D2GameBatchC3630Env* e) { return c4d30(e, p1, p2, 0xd); }
// 1.13c D2Game+0xC5030
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_C5030(uint32_t p1, uint32_t p2, const D2GameBatchC3630Env* e) { return c4d30(e, p1, p2, 0xc); }

// 1.13c D2Game+0xC5100
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_fn_d2game_C5100(uint32_t p1, uint32_t unit, const D2GameBatchC3630Env* e)
{
    (void)p1;
    if (unit != 0 && rd32(unit, 0) == 0) {
        uint32_t r = e->ordinal10920(unit);
        uint32_t pl = rd32(r, 0x9c);
        if (pl != 0) {
            if (rd8(pl, 0x1ac) == 1) {
                uint8_t buf[10] = {};
                { const uint32_t k = 0x253; std::memcpy(buf, &k, 4); }
                e->sub8A3E0(buf, 10);
            }
            return 0;
        }
    }
    uint32_t v = e->ordinal10265(0x7cb);
    e->ordinal10024(e->assertFile, v);
    std::exit(-1);
}
