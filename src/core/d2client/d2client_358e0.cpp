#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x358E0: when the dword at 0x11C44C is set, calls 0xC2790(0x15, 1, 0) and clears it.
// Returns 0 (void).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_358e0(uint32_t* g44c, void (*c2790)(uint32_t, uint32_t, uint32_t)) {
    if (*g44c) { c2790(0x15, 1, 0); *g44c = 0; }
    return 0;
}
