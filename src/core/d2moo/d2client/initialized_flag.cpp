#include <cstdint>
#include <emscripten.h>

namespace {
// D2Client+0x62E20 returns this global unchanged. The client-data initializer is
// implemented by a separate function; this state is its wasm-side storage.
uint32_t g_clientDataInitialized = 0;
}

// 1.13c D2Client+0x62E20
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_10001(uint32_t ignored) {
    (void)ignored;
    return g_clientDataInitialized;
}

// Scenario setup: FUNCTION vectors pin the retail global before invoking the getter.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_10001_pin(uint32_t value) {
    g_clientDataInitialized = value;
}
