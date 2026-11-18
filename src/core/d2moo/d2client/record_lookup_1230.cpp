#include <cstdint>
#include <emscripten.h>

namespace {
uint32_t g_base1230 = 0, g_count1230 = 0;
}

// 1.13c D2Client+0x1230: monstats record (stride 0x1A8) from sgptDataTables+0xA78 / count +0xA80.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1230(int32_t index) {
    if (index >= 0 && static_cast<uint32_t>(index) < g_count1230) return g_base1230 + static_cast<uint32_t>(index) * 0x1A8u;
    return 0;
}

// FUNCTION replay pins the table base and count (retail reads them from sgptDataTables).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1230_pin(uint32_t base, uint32_t count) { g_base1230 = base; g_count1230 = count; }
