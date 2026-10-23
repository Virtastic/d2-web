// D2Client+0xADEB0: packet handler; param_1 = packet bytes (fastcall ECX). env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cADEB0Env {
    int32_t* (*lookup)(int32_t id, void* tbl, uint32_t type);  // D2Client+0xA4E20
    void (*f73760)(uint32_t b);                                // D2Client+0x73760
    void (*f72530)(uint32_t b);                                // D2Client+0x72530
    uint8_t* tbl;                                              // DAT_6fbba608 (RVA 0xBA608), 0x80/entry
};
extern "C" {
// 1.13c D2Client+0xADEB0
EMSCRIPTEN_KEEPALIVE void d2c_adeb0(const D2cADEB0Env* e, const uint8_t* p) {
    uint32_t t = p[1];
    int32_t id; __builtin_memcpy(&id, p + 2, 4);  // unaligned dword at +2
    if (e->lookup(id, e->tbl + t * 0x80, t)) { e->f73760(p[6]); e->f72530(p[6]); }
}
}
