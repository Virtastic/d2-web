#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x12B0 / 0x1840 / 0x1870 / 0x18A0: sgptDataTables record lookups. Each loads the tables
// pointer through the D2Common data import (D2Client+0xCE474 -> cell -> tables), compares the signed
// index against a count and returns array + index*stride, else 0. Callers pass the already-read count/array.
static uint32_t lookup(int32_t index, int32_t count, uint32_t array, uint32_t stride) {
    return (index >= 0 && index < count) ? array + static_cast<uint32_t>(index) * stride : 0;  // signed compare
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_12b0(int32_t i, int32_t n, uint32_t a) { return lookup(i, n, a, 0x3C); }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1840(int32_t i, int32_t n, uint32_t a) { return lookup(i, n, a, 0x128); }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1870(int32_t i, int32_t n, uint32_t a) { return lookup(i, n, a, 0x1B8); }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_18a0(int32_t i, int32_t n, uint32_t a) { return lookup(i, n, a, 0x14C); }
