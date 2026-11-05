// 1.13c D2Client+0xA4C50 (70 B, ESI = unit, one unused stack word, ret 4): 1 when ordinal 10xxx (0xC310) finds no
// record; 0 when the record lookup (0xC640) fails; for a record of kind 0x12 whose byte +0x16 is neither 0 nor 0xFF,
// (unit +0x38 >> 8, arithmetic) >= that byte; any other record answers 1.
#include <emscripten.h>
#include <cstdint>
extern "C" uint32_t d2c_port_a4c50(uint32_t unit, uint32_t (*c310)(uint32_t), uint32_t (*c640)(uint32_t)) {
    uint32_t h = c310(unit);
    if (h == 0) return 1;
    uint32_t rec = c640(h);
    if (rec == 0) return 0;
    const uint8_t* b = (const uint8_t*)(uintptr_t)rec;
    if (b[0x10] != 0x12) return 1;
    uint32_t v = b[0x16];
    if (v == 0 || v == 0xFF) return 0;
    return ((int32_t)*(const uint32_t*)(uintptr_t)(unit + 0x38) >> 8) >= (int32_t)v ? 1u : 0u;
}
