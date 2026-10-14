#include <cstdint>
#include <emscripten.h>

extern "C" uint32_t d2_client_10001(uint32_t ignored);

namespace {
constexpr uint32_t kClientByteCount = 37;
uint8_t g_clientBytes[kClientByteCount]{};
}

// 1.13c D2Client+0x62DE0, export ordinal 10002.
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t d2_client_10002(uint32_t ignored, int32_t index) {
    (void)ignored;
    if (d2_client_10001(0) == 0 || index < 0 || static_cast<uint32_t>(index) >= kClientByteCount) {
        __builtin_trap();
    }
    return g_clientBytes[static_cast<uint32_t>(index)];
}

// FUNCTION replay pins the indexed byte from the same captured retail state.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_10002_pin(uint32_t index, uint32_t value) {
    if (index < kClientByteCount) g_clientBytes[index] = static_cast<uint8_t>(value);
}
