#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x17B0: EAX = skill id. sgptDataTables+0xB98 (skills array, stride 0x23C) / +0xBA0 (count); s16 at record+0x12C, default 20 when out of range or < 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_17b0(int32_t idx, uint32_t count, int32_t field, uint32_t inRange) {
    if (idx < 0 || (int32_t)count <= idx || !inRange) return 20;
    int32_t v = (int16_t)field;
    return v < 1 ? 20u : (uint32_t)v;
}
