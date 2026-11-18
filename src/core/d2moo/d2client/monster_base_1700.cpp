#include <cstdint>
#include <emscripten.h>

namespace {
uint32_t g_base1700 = 0;   // sgptDataTables+0xA78 monstats records, stride 0x1A8
int32_t g_count1700 = 0;   // sgptDataTables+0xA80 record count
}

extern "C" {
// 1.13c D2Client+0x1700: EAX = descriptor pointer (+0 kind, +4 monstats index), result in EAX.
// Null or kind != 1 -> -1. Index outside [0, count) (signed) -> the index itself. Otherwise the
// record's i16 nBaseId at +2; a record pointer that computes to 0 also returns the index.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_1700(uint32_t desc) {
    if (!desc) return -1;
    const int32_t* d = reinterpret_cast<const int32_t*>(static_cast<uintptr_t>(desc));
    if (d[0] != 1) return -1;
    const int32_t id = d[1];
    if (id < 0 || id >= g_count1700) return id;
    const uint32_t rec = g_base1700 + static_cast<uint32_t>(id) * 0x1A8u;
    if (!rec) return id;
    return static_cast<int16_t>(reinterpret_cast<const int16_t*>(static_cast<uintptr_t>(rec))[1]);  // +2
}

// FUNCTION replay pins the two sgptDataTables fields this function reads.
EMSCRIPTEN_KEEPALIVE void d2_client_1700_pin(uint32_t base, int32_t count) {
    g_base1700 = base; g_count1700 = count;
}
}
