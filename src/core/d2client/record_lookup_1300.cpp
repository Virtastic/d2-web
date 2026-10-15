#include <cstdint>
#include <emscripten.h>

namespace {
uint32_t g_base1300 = 0, g_count1300 = 0;
}

// 1.13c D2Client+0x1300: record (stride 0xC4) from sgptDataTables+0xBC4 / count +0xBC8, signed range check.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1300(int32_t index) {
    if (index >= 0 && static_cast<uint32_t>(index) < g_count1300) return g_base1300 + static_cast<uint32_t>(index) * 0xC4u;
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1300_pin(uint32_t base, uint32_t count) { g_base1300 = base; g_count1300 = count; }
