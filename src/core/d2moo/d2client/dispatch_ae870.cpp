// D2Client+0xAE870: 0x101-byte record dispatcher on byte[1]; env-seam style.
// Ordinal_10265 / Ordinal_10024 numbers are unverified (taken from the decompile only).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2cAE870Env {
    int32_t (*ord10265)(int32_t id);            // Ordinal_10265 (string by id)
    void (*ord10024)(const void* fmt, int32_t s);  // Ordinal_10024 (fatal message); then _exit(-1)
    const void* fmt_8359f;                      // DAT D2Client+0x1359F (RVA of 6fb8359f)
    void (*exit_fn)(int32_t code);              // D2Client+0x3664 (_exit)
    // local record pointer: EAX 344f0 35ed0 35710 33880 354e0 33790, ESI 369e0 369b0, stack word 36540 361a0 34f00
    void (*f344f0)(void* rec);   void (*f36540)(void* rec);  void (*f361a0)(void* rec);
    void (*f35ed0)(void* rec);   void (*f369e0)(void* rec);           void (*f35710)(void* rec);
    void (*f33880)(void* rec);   void (*f354e0)(void* rec);           void (*f34f00)(void* rec);
    void (*f33790)(void* rec);   void (*f369b0)(void* rec);
};
extern "C" {
// 1.13c D2Client+0xAE870
EMSCRIPTEN_KEEPALIVE void d2c_ae870(const D2cAE870Env* e, const uint8_t* src) {
    uint8_t rec[0x101];
    memcpy(rec, src, sizeof rec);
    switch (rec[1]) {
    case 0: case 1: case 2: case 3: case 4: case 10: case 0xb: case 0xc:
    case 0xd: case 0xe: case 0xf: case 0x10: case 0x12:
        e->ord10024(e->fmt_8359f, e->ord10265(0xfb7));
        e->exit_fn(-1);
        return;
    case 5: e->f344f0(rec); return;
    case 6: e->f36540(rec); return;
    case 7: e->f361a0(rec); return;
    case 8: e->f35ed0(rec); return;
    case 9: e->f369e0(rec); return;
    case 0x11: e->f35710(rec); return;
    case 0x13: e->f33880(rec); return;
    case 0x14: e->f354e0(rec); return;
    case 0x15: e->f34f00(rec); return;
    case 0x16: e->f33790(rec); return;
    case 0x17: e->f369b0(rec); return;
    }
}
}
