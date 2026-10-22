// src/core/d2win/d2win_c410.cpp -- 1.13c D2Win+0x0C410 (ord 10093, 142 B), from the cached decompile.
// Looks id up in the list at *(w+0xB8) (node: +0 -> entry, +4 next; entry +4 = id, +0x14 = data).
// Hit with data: w[0x20]=id, w[0xC]=data[1], w[8]=0, w[0x10]=data[0x18], w[5]=data[2]; returns 1. No hit: 0.
// NULL w (code 0x8B4) or hit with NULL data (0x3D2) is fatal: Ordinal_10265(code) text, Ordinal_10024, _exit(-1).
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2WinC410Env { void (*fatal)(uint32_t code); };

static uint32_t rd(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
static void wr(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }

// 1.13c D2Win+0x0C410
int32_t D2Win_C410(uint8_t* w, uint32_t id, const D2WinC410Env& env, uint8_t* (*deref)(uint32_t)) {
    if (!w) { env.fatal(0x8B4); std::exit(-1); }
    uint32_t node = rd(deref(rd(deref(rd(w + 0xB8)))));
    for (;;) {
        if (!node) return 0;
        if (rd(deref(rd(deref(node))) + 4) == id) break;
        node = rd(deref(node) + 4);
    }
    uint32_t dp = rd(deref(rd(deref(node))) + 0x14);
    if (!dp) { env.fatal(0x3D2); std::exit(-1); }
    const uint8_t* d = deref(dp);
    wr(w + 0x20, id);
    wr(w + 0xC, d[1]);
    wr(w + 8, 0);
    wr(w + 0x10, rd(d + 0x18));
    w[5] = d[2];
    return 1;
}
