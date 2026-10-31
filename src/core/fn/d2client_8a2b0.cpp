// 1.13c D2Client+0x8a2b0 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" void d2_client_8a2b0(uint32_t, void (*)(uint32_t, uint32_t, uint32_t));
static uint32_t g_called;
static void vfree(uint32_t p, uint32_t sz, uint32_t ty) { if (sz == 0 && ty == 0x8000 && p) g_called = 1; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_8a2b0(uint32_t* r) {
    g_called = 0; d2_client_8a2b0(r[0], vfree); r[4] = g_called ? 0x55u : r[0];
}
