// 1.13c D2Client+0xA0F60: EAX = index into the 0x1A4-stride table (sgptDataTables +0xB64, count +0xB6C), ECX = p1.
// Builds a record through D2Client+0xA8C20 (EBX=1, five stack words) and submits it through thunk C5C2.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cA0f60Env {
    uint32_t (*f8c20)(uint32_t one, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);   // EBX=one, stack a..e (ret 0x14)
    uint32_t (*ord10327)(uint32_t p1, uint32_t rec);                                               // thunk C5C2
    void (*f8630)(uint32_t rec);                                                                   // ESI = rec
    void (*fatal)(uint32_t code);
};
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_a0f60(const D2cA0f60Env* e, const uint8_t* dt, int32_t idx, uint32_t p1, uint8_t* (*zptr)(uint32_t)) {
    uint32_t cnt, base; memcpy(&cnt, dt + 0xb6c, 4); memcpy(&base, dt + 0xb64, 4);
    if (idx < 0 || idx >= (int32_t)cnt) return;   // retail reads +0x130 of NULL here
    const uint8_t* rec = (const uint8_t*)(uintptr_t)base + (size_t)idx * 0x1a4;
    uint32_t v = rec[0x130];
    if (!v) return;
    uint32_t zh = e->f8c20(1, 1, 0xff, rec[0x132], rec[0x133], rec[0x134]);
    if (v >= 0x12) v = 0x12;
    if (!zh) { e->fatal(0x131); return; }
    uint32_t t = v << 3;
    memcpy(zptr(zh) + 0x1c, &t, 4);
    uint32_t r = e->ord10327(p1, zh);
    if (r) e->f8630(r);
}
}
