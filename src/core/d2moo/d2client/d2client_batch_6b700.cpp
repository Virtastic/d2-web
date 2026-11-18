// src/core/d2client/d2client_batch_6b700.cpp -- 1.13c D2Client+0x6B700 (903 B), from the disassembly (the decompile hides the
// EAX = unit register argument and reuses stack slots). Imports and D2Client-internal callees are host callbacks with the
// exact argument order seen at each call site. Globals are parameters (D2Client .data: cc414, 8bc48, c995c, c9960, cc418).
#include <cstdint>
#include <emscripten.h>

namespace {
inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
}
struct H6b700 {
    int32_t (*gfx10013)();                                           // D2gfx#10013 (render-mode query), 0 = branch A
    int32_t (*cm10651)(uint32_t u);                                  // D2Common#10651
    int32_t (*cm11142)(uint32_t u);                                  // D2Common#11142
    int32_t (*cm11168)(uint32_t u, int32_t v, int32_t* o4, int32_t* o8);
    int32_t (*cm11080)(uint32_t u, int32_t r);
    void    (*gfx10023)(int32_t r);
    int32_t (*cm10175)(uint32_t u);
    void    (*cm10350)(uint32_t u, int32_t* type, int32_t* id, int32_t* sub);   // + file string, line 0xA2F
    uint8_t (*cm10663)(uint32_t u);
    void    (*cm10569)(uint32_t u, int32_t* a, int32_t* b);
    int32_t (*cm10866)(uint32_t u, int32_t k);
    int32_t (*cm10431)(uint32_t u, int32_t w, int32_t* out, int32_t sub, int32_t z);
    void    (*gfx10046)(const uint32_t* pk, int32_t x, int32_t y, uint8_t al);
    uint32_t (*d560d0)(uint32_t u);                                  // eax = u
    void    (*d6a860)(uint32_t u);                                   // eax = u
    int32_t (*d6a3d0)(uint32_t u, int32_t x, int32_t y, int32_t z);  // eax = u, stack x,y,0
    uint8_t (*d18cf0)();
    uint32_t (*d19910)(uint32_t u, int32_t m1);                      // eax = u, ecx = -1
    uint32_t (*d1a2f0)(int32_t m1, int32_t r431, uint32_t u);        // eax = -1, ecx = r431, edx = u
    const uint8_t* (*d19090)(uint32_t u, int32_t sub);               // eax = u, ecx = sub
    void    (*d1dca0)(uint32_t u, int32_t type, int32_t id, int32_t sub, int32_t* o10, int32_t* o14, int32_t* o28, uint8_t rec2, int32_t one);
    uint8_t* (*d1d6d0)(const uint8_t* rec, int32_t v14, int32_t v20);   // eax = rec, stack v14, v20
    int32_t (*d1ea50)(const uint8_t* rec, uint8_t* tbl, int32_t* idx, int32_t v10, int32_t v20);
    int32_t (*d1e670)(uint32_t u, uint8_t b);                        // eax = u, stack b
    int32_t (*d1e2c0)(uint32_t u, uint32_t* pk, int32_t* o38);       // eax = -1
    void    (*fatal)(uint32_t code);                                 // Fog#10265(0xA56), Fog#10024, exit(-1)
};

// 1.13c D2Client+0x6B700 (eax = unit). Returns the eax at exit. Node fields: unit+0 type, +4 id, +0x54 path, +0x60, +0xC8 flags.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_6b700(uint32_t u, const H6b700* h, int32_t cc414, int32_t g8bc48, int32_t g995c, int32_t g9960, int32_t gc418) {
    const int32_t mode = h->gfx10013();
    if (!u || (int32_t)rd32(u) < 0 || (int32_t)rd32(u) > 2) { h->d6a860(u); return 1; }
    const uint32_t path = rd32(u + 0x54), p30 = path ? rd32(path + 0x30) : 0;
    auto f = [&](uint32_t off) -> int32_t { return p30 ? (int32_t)rd32(p30 + off) : 0; };
    int32_t L4, L8;
    if (mode != 0) {
        int32_t o4 = 0, o8 = 0;
        int32_t r1 = h->cm11168(u, f(0x3c), &o4, &o8);
        h->gfx10023(h->cm11080(u, r1));
        int32_t q = (g8bc48 + ((g8bc48 >> 31) & 3)) >> 2;
        if (cc414 == 1) o4 -= q; else if (cc414 == 2) o4 += q;
        L4 = o4 + f(0x34);
        L8 = o8 + 0x10 + f(0x38);
    } else {
        L4 = h->cm10651(u) + (f(0x34) - (g9960 - gc418));
        L8 = h->cm11142(u) + (f(0x38) - (g995c - 8)) + f(0x3c);   // the +0x3c term is added after the store, as at b843
    }
    if (!h->d6a3d0(u, L4, L8, 0)) return 0;
    const uint8_t pkAL = h->d18cf0();                         // low byte only; retail pushes the dword, upper bytes are stack garbage
    int32_t type = (int32_t)rd32(u), id = (int32_t)rd32(u + 4);
    int32_t sub = h->cm10175(u);
    h->cm10350(u, &type, &id, &sub);
    int32_t F10 = h->cm10663(u), a20 = 0, a30 = 0;
    h->cm10569(u, &a20, &a30);
    const uint32_t flag3 = (rd32(u + 0xc8) >> 3) & 1;
    int32_t w = (int32_t)rd32(u + 0x60);
    if (flag3 && h->cm10866(u, 0x25)) { uint32_t r = h->d560d0(u); if (r) w = (int32_t)rd32(r + 0x60); }
    int32_t a34 = 0;
    const int32_t r431 = h->cm10431(u, w, &a34, sub, 0);
    if (!h->d19910(u, -1) && !h->d1a2f0(-1, r431, u)) return 0;   // results beyond non-zero are unused
    const uint8_t* rec = h->d19090(u, sub);
    if (!rec) return 0;
    int32_t F14 = 0, F28 = (int32_t)flag3;
    h->d1dca0(u, type, id, sub, &F10, &F14, &F28, rec[2], 1);
    uint8_t* tbl = h->d1d6d0(rec, F14, a20);
    if (!tbl) { h->fatal(0xa56); return 0; }                  // retail: Fog#10024(msg, Fog#10265(0xA56)); exit(-1)
    int32_t idx = 0;
    if (h->d1ea50(rec, tbl, &idx, F10, a20) <= 0) return 0;
    for (idx = 0; idx < (int32_t)rec[0]; ++idx) {
        const uint8_t b = tbl[idx];
        if (!h->d1e670(u, b)) continue;
        uint32_t pk[0x12] = {0};
        int32_t o38 = 0;
        pk[0] = (uint32_t)a20; pk[1] = (uint32_t)b << 8; pk[16] = (uint32_t)F10;
        if (h->d1e2c0(u, pk, &o38)) h->gfx10046(pk, L4, L8, pkAL);
    }
    return 1;
}
