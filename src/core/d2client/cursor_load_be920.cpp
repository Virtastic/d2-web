// D2Client+0xBE920: load the 13 table entries (stride 12 at +0x806DC), plus 0x20A when a mouse wheel is present; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cBE920Env {
    int32_t (*ord10048)();            // unverified ordinal; 0 -> fatal (Ordinal_10265(0x179) message, _exit(-1))
    void (*fatal)();                  // Ordinal_10265(0x179)+Ordinal_10024+_exit(-1)
    void (*load)(int32_t id);         // FUN at D2Client+0x6CA80
    uint32_t table[13];               // dword at D2Client+0x806DC + i*12
    int32_t wheel;                    // GetSystemMetrics(0x4B)
    int32_t g3cc, g898, g890, g88c;   // D2Client+0xCC3CC, +0xC9898, +0xC9890, +0xC988C
};
extern "C" {
// 1.13c D2Client+0xBE920
EMSCRIPTEN_KEEPALIVE void d2c_be920(const D2cBE920Env* e) {
    if (!e->ord10048()) { e->fatal(); return; }
    for (int i = 0; i < 13; i++) e->load((int32_t)e->table[i]);
    if (!e->wheel || !e->g3cc) return;
    if (e->g898 == 1 || (e->g898 == 2 && (e->g88c == 4 || e->g88c == 5))) {
        if (!e->ord10048()) { e->fatal(); return; }
        e->load(0x20a);
    }
}
}
