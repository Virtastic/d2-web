// D2Client+0x38A20: constructor of a wrapper object holding a 0x5A-byte child at +8; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c38A20Env {
    void* (*fogAlloc)(int32_t size, int32_t file, int32_t line, int32_t zero);  // Ordinal_401 (file=&DAT_6fb804f0, line=-1)
    void* (*f38820)(void* mem, uint32_t a, uint16_t b, uint32_t c);             // D2Client+0x38820
    uint32_t vtblBase;   // &PTR_LAB_6fb85048
    uint32_t vtblFinal;  // &PTR_FUN_6fb86870
};
extern "C" {
// 1.13c D2Client+0x38A20
EMSCRIPTEN_KEEPALIVE uint32_t* d2c_38a20(const D2c38A20Env* e, uint32_t* self, uint32_t a, uint16_t b, uint32_t c) {
    self[0] = e->vtblBase;
    self[0] = e->vtblFinal;
    void* m = e->fogAlloc(0x5a, 0, -1, 0);
    self[2] = m ? (uint32_t)(uintptr_t)e->f38820(m, a, b, c) : 0;
    return self;
}
}
