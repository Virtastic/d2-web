#include <cstdint>
#include <emscripten.h>

namespace {
uint32_t g_base15b0 = 0, g_count15b0 = 0;
}

// 1.13c D2Client+0x15B0: record (stride 0x23C) from sgptDataTables+0xB98 / count +0xBA0, signed range check.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_15b0(int32_t index) {
    if (index >= 0 && static_cast<uint32_t>(index) < g_count15b0) return g_base15b0 + static_cast<uint32_t>(index) * 0x23Cu;
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_15b0_pin(uint32_t base, uint32_t count) { g_base15b0 = base; g_count15b0 = count; }
