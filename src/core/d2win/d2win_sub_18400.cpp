// src/core/d2win/d2win_sub_18400.cpp -- 1.13c D2Win+0x18400 (143 B), from the cached decompile.
// D2Win init: sub-inits, clear g_9014bc, get a handle (Ordinal_10048), fatal if 0, register the
// 0x180/12 = 32 table triples (a,b,c) via sub_11240(handle,a,b,c), sub_ef2d0(0), clear g_a9cf0, return 1.
#include <cstdint>
#include <cstdlib>

struct D2Win18400Env {
    void (*f3110)(); void (*f2fe0)(int32_t); void (*f6990)(); void (*fbe00)();
    uint32_t* g9014bc; uint32_t* ga9cf0;
    int32_t (*getHandle)();                 // Ordinal_10048
    void (*fatal)(uint32_t code);           // Ordinal_10265(0x4d) text, Ordinal_10024("", text)
    void (*reg)(int32_t h, uint32_t a, int32_t b, uint32_t c);  // sub_11240
    const uint32_t* table;                  // DAT_6f8ffbb0, 32 x {a,b,c}
    void (*f2d0)(uint32_t);
};

// 1.13c D2Win+0x18400
int32_t D2Win_18400(const D2Win18400Env& e) {
    e.f3110(); e.f2fe0(1); e.f6990(); e.fbe00();
    *e.g9014bc = 0;
    int32_t h = e.getHandle();
    if (!h) { e.fatal(0x4d); std::exit(-1); }
    for (uint32_t i = 0; i < 0x180; i += 12) {
        const uint32_t* t = e.table + i / 4;
        e.reg(h, t[0], (int32_t)t[1], t[2]);
    }
    e.f2d0(0);
    *e.ga9cf0 = 0;
    return 1;
}
