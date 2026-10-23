// D2Client+0xA51F0: register-arg predicate (EAX=a, EDI=b, int32 dword arrays); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA51F0Env {
    int32_t (*ord10828)();                       // Ordinal_10828 (unverified ordinal -> id; 0 = none)
    int32_t (*ord10507)();                       // Ordinal_10507 (unverified ordinal -> id; 0 = none)
    void (*ord10601)(int32_t v, const char* file, int32_t line);  // Ordinal_10601 (unverified)
    const uint8_t* (*f15B0)(int32_t id);         // D2Client+0x15B0: record (0x23C stride) or null
    int32_t (*f216A0)(int32_t key);              // D2Client+0x216A0: list lookup, -1 if absent
    uint32_t bitMask4;                           // gdwBitMasks[4]
};
extern "C" {
// 1.13c D2Client+0xA51F0
EMSCRIPTEN_KEEPALIVE int32_t d2c_a51f0(const D2cA51F0Env* e, const int32_t* a, const int32_t* b) {
    static const char* F = "..\\Source\\D2Client\\UNIT\\CUnit.cpp";
    bool r = false;
    if (a && b && (a[0] == 0 || b[0] == 1)) {
        int32_t id = e->ord10828();
        if (id) {
            e->ord10601(id, F, 0xb4a);
            const uint8_t* p = e->f15B0(id);
            if (p && ((uint8_t)e->bitMask4 & p[7])) r = true;
        }
        bool hit = false;
        id = e->ord10507();
        if (id) {
            e->ord10601(id, F, 0xb55);
            const uint8_t* p = e->f15B0(id);
            if (p && ((uint8_t)e->bitMask4 & p[7])) hit = true;
        }
        if (hit || r) return e->f216A0(b[3]) == a[3];
    }
    return 0;
}
}
