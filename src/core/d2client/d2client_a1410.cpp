// D2Client+0xA1410 / +0xA1540 / +0xA15C0: register-convention helpers around the unit-size getter D2Client+0xA1340.
// Disassembly-derived (the decompile hid the register and stack arguments); every callee is an env seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cA1410Env {
    uint32_t (*ord10331)(uint32_t p1);                                   // thunk C1F0
    uint32_t (*a1340)(uint32_t unit, uint32_t* w, uint32_t* h);         // EBX=unit, EDI=&w, stack &h
    uint32_t (*ord10303)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);  // thunk C712 (6 stack words)
    void (*fatal)(uint32_t code);                                        // Ordinal_10265 + 10024 + _exit(-1)
};
static uint32_t rd32(const uint8_t* p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
extern "C" {
// 1.13c D2Client+0xA1410: EAX=index into the 0x1A4-stride table (sgptDataTables +0xB64, count +0xB6C), ECX=p1.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_a1410(const D2cA1410Env* e, const uint8_t* dt, int32_t idx, uint32_t p1) {
    const uint8_t* rec = nullptr;
    if (idx >= 0 && idx < (int32_t)rd32(dt, 0xb6c)) rec = (const uint8_t*)(uintptr_t)rd32(dt, 0xb64) + (size_t)idx * 0x1a4;
    uint32_t q = e->ord10331(p1);
    if (!rec) { e->fatal(0x9be); return 0; }
    if (!q) return 0;
    uint32_t w = 0, h = 0;
    e->a1340(p1, &w, &h);
    if (!w || !h) return 0;
    uint32_t b = rec[0x18a];
    return (uint16_t)e->ord10303(q, w, h, b, b, 5) == 0;
}
}

struct D2cA1540Env {
    uint32_t (*a1340)(uint32_t unit, uint32_t* w, uint32_t* h);
    uint32_t (*f1660)(uint32_t unit);     // ECX=unit
    uint32_t (*f1630)(uint32_t unit);     // ECX=unit
    int32_t (*ord10769)(uint32_t x, uint32_t y, uint32_t w, uint32_t h);   // thunk C6FA
    uint32_t (*faff10)(uint32_t unit, uint32_t a0, uint32_t w, uint32_t h, uint32_t a1, uint32_t a2);  // EDX=unit, ret 0x14
    uint32_t (*f60450)(uint32_t unit, uint32_t a0, uint32_t w, uint32_t h, uint32_t a1, uint32_t a2);  // ESI=unit, ret 0x14
};
extern "C" {
// 1.13c D2Client+0xA1540: EAX=unit, stack a0,a1,a2 (ret 0xC).
EMSCRIPTEN_KEEPALIVE uint32_t d2c_a1540(const D2cA1540Env* e, uint32_t unit, uint32_t a0, uint32_t a1, uint32_t a2) {
    uint32_t w = 0, h = 0;
    if (!e->a1340(unit, &w, &h)) return 0;
    if (w && h) {
        uint32_t y = e->f1660(unit);
        uint32_t x = e->f1630(unit);
        if (e->ord10769(x, y, w, h) <= 0x2710) return e->faff10(unit, a0, w, h, a1, a2);
    }
    return 0;
}
// 1.13c D2Client+0xA15C0: EAX=unit, stack a0,a1,a2 (ret 0xC).
EMSCRIPTEN_KEEPALIVE uint32_t d2c_a15c0(const D2cA1540Env* e, uint32_t unit, uint32_t a0, uint32_t a1, uint32_t a2) {
    uint32_t w = 0, h = 0;
    if (!e->a1340(unit, &w, &h) || !w || !h) return 0;
    return e->f60450(unit, a0, w, h, a1, a2);
}
}
