// D2Client+0x2A6C0: register-arg helper (ESI=unit dword array, EDI=arg); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA6C0Env {
    int32_t (*ord11090)();                 // Ordinal_11090
    void (*f6A220)();                      // D2Client+0x6A220
    void (*f2A590)(void* edi, int32_t* esi);  // D2Client+0x2A590
    void (*f24490)(void* edi, int32_t zero);  // D2Client+0x24490
};
extern "C" {
// 1.13c D2Client+0x2A6C0
EMSCRIPTEN_KEEPALIVE void d2c_2a6c0(const D2cA6C0Env* e, int32_t* esi, void* edi) {
    if (esi) esi[0x31] &= 0xffffffbf;
    if (e->ord11090()) { e->f6A220(); e->f2A590(edi, esi); e->f24490(edi, 0); }
}
}
