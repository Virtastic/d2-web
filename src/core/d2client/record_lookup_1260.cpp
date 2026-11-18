#include <cstdint>
#include <emscripten.h>

namespace {
uint32_t g_base1260 = 0, g_count1260 = 0, g_count2_1260 = 0;
int32_t g_field1260 = 0;
}

// 1.13c D2Client+0x1260: monstats record's i16 at +0x18 (monstats2 index) selects a 0x134-byte
// monstats2 record from sgptDataTables+0xA90 (count +0xA98). Both range checks are signed.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1260(int32_t index) {
    if (index < 0 || static_cast<uint32_t>(index) >= g_count1260) return 0;
    const int32_t f = static_cast<int16_t>(g_field1260);
    if (f < 0 || static_cast<uint32_t>(f) >= g_count2_1260) return 0;
    return g_base1260 + static_cast<uint32_t>(f) * 0x134u;
}

// FUNCTION replay pins monstats2 base, both counts and the monstats[index]+0x18 value.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1260_pin(uint32_t base, uint32_t count, uint32_t count2, int32_t field) {
    g_base1260 = base; g_count1260 = count; g_count2_1260 = count2; g_field1260 = field;
}
