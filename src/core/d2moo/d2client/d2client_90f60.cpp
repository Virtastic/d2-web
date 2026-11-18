// 1.13c D2Client+0x90F60 (fastcall, ECX = key for Ordinal 10227; DAT_6fbc9854 = g9854; DAT_6fbcbbfc = unit): when 10227 answers and g9854 is set,
// 88A70(eax = g9854, ebx = 1, four zero words), the one-byte packet 0x60 (143E0: stack buf, EBX = 1), then for k = 4, 5:
// 11139(unit[0x60], k) nonzero: 81CE0(eax = that item).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_90f60(const D2cSvc& v, uint32_t ecx, uint32_t g9854, uint32_t unit) {
    if (d2c_call(v, 0xbf8c, 0, ecx, 0, 0, 0) == 0 || g9854 == 0) return;
    d2c_call(v, 0x88a70, g9854, 0, 0, 1, 0, 0, 0, 0, 0);
    uint8_t pkt[4] = { 0x60, 0, 0, 0 };
    d2c_call(v, 0x143e0, 0, 0, 0, 1, 0, (uint32_t)(uintptr_t)pkt);
    uint32_t u; memcpy(&u, (const uint8_t*)(uintptr_t)unit + 0x60, 4);
    uint32_t it = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, u, 4);
    if (it) d2c_call(v, 0x81ce0, it, 0, 0, 0, 0);
    it = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, u, 5);
    if (it) d2c_call(v, 0x81ce0, it, 0, 0, 0, 0);
}
