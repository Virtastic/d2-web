// src/core/d2cmp/d2cmp_def0.cpp -- 1.13c D2CMP.dll+0xDEF0, transcribed from the disassembly (the decompile drops the register arguments).
// Retail convention: stdcall, one stack word (the cel context p), ret 4. Resolves a context's cel (p[0xd] set) or the sprite-cache entry for its file:
// D350 (EAX = &name[0x104], stack p, &flag) returns the file record r; the cache lookup 0x17020 (ECX = r, EDX = &Y, EDI = &X, stack key) returns an entry or null.
// With no entry and Y > 1, X > 0 and r's flag bit 0 clear the frame divider is rescaled through 0x121C0 and the lookup repeats (fatal 0x7D2/0x7D3 if it fails).
#include <cstdint>
#include <cstring>
#include <unistd.h>

struct D2CMPDEF0Env {
    uint32_t (*c122e0)(uint8_t* p);                                           // 0x122E0: GetCelFromCelContext, stack p
    uint8_t* (*d350)(uint8_t* name, uint8_t* p, uint8_t* flagOut);            // 0xD350: EAX name buffer, stack p, &flagOut
    uint8_t* (*c17020)(uint8_t* r, uint32_t* y, uint32_t* x, uint32_t key);   // 0x17020: ECX r, EDX &Y, EDI &X, stack key (0x672B0)
    uint32_t (*c121c0)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);       // 0x121C0: stack (a, b, c, d)
    uint32_t (*cc2d0)(uint8_t* name, uint8_t* flagOut);                       // 0xC2D0: EBX name, stack &flagOut
    uint32_t (*dc80)(uint8_t* p, uint8_t* r, uint32_t x, uint32_t y, uint8_t* flagOut);   // 0xDC80: stack (p, r, X, Y, &flagOut)
};

static inline uint32_t df_rd(const uint8_t* b, uint32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; }
static inline void df_wr(uint8_t* b, uint32_t o, uint32_t v) { std::memcpy(b + o, &v, 4); }

// 1.13c D2CMP+0x0DEF0
extern "C" uint32_t d2cmp_def0(uint8_t* p, const D2CMPDEF0Env* e) {
    if (!p) _exit(-1);                                                         // fatal 0x797
    if (df_rd(p, 0x34)) { df_wr(p, 0x3c, e->c122e0(p)); return 1; }
    uint8_t name[0x104]; uint8_t flagOut[4];
    uint32_t x = 0, y = 0;
    uint8_t* r = e->d350(name, p, flagOut);
    if (!r) return 0;
    uint8_t* ent = e->c17020(r, &y, &x, 0x672b0u);
    if (ent) {
        uint8_t* t = (uint8_t*)(uintptr_t)df_rd(ent, 0x20);
        if (t[0x20] & 1) df_wr(p, 0x44, df_rd(p, 0x44) | 4);
        return 1;
    }
    if (y > 1 && x > 0 && !(r[0] & 1)) {
        const uint32_t per = df_rd(r, 0x14);
        uint32_t v = e->c121c0((df_rd(p, 0x40) * per) >> 6, per, y, p[4] & 4);
        const uint32_t per2 = df_rd(r, 0x14);
        v = per2 > 1 ? (v << 6) / per2 : 1;
        df_wr(p, 0x40, v);
        ent = e->c17020(r, &y, &x, 0x672b0u);
        if (!ent || x != 0) _exit(-1);                                         // fatal 0x7D2 / 0x7D3
    }
    if (flagOut[0] == 0 && !e->cc2d0(name, flagOut)) return 0;
    return e->dc80(p, r, x, y, flagOut);
}
