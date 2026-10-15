#include <cstdint>
#include <emscripten.h>

namespace {
uint32_t g_base1580 = 0, g_count1580 = 0;
}

// 1.13c D2Client+0x1580: record (stride 0x1A4) from sgptDataTables+0xB64 / count +0xB6C, signed range check.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1580(int32_t index) {
    if (index >= 0 && static_cast<uint32_t>(index) < g_count1580) return g_base1580 + static_cast<uint32_t>(index) * 0x1A4u;
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1580_pin(uint32_t base, uint32_t count) { g_base1580 = base; g_count1580 = count; }
