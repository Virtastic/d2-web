// 1.13c D2Common+0x4DFA0 sub_4DFA0, +0x7E390 sub_7E390, +0x30420 sub_30420, +0x303A0 sub_303A0, +0x30460 sub_30460
//. stdcall, one stack arg (ret 4; sub_7E390 ret 0xC). Null arg: assert + _exit, not probed.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
static const uint32_t* u32(uint32_t a) { return (const uint32_t*)(uintptr_t)a; }
static uint32_t pA8(uint32_t unit) { return u32(unit)[0xA8 / 4]; }   // unit+0xA8 (image of the 32-bit layout)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_4dfa0(uint32_t* r) { r[4] = u32(r[0])[0x24 / 4]; }   // 1.13c d2common+0x4DFA0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7e390(uint32_t* r) {   // 1.13c d2common+0x7E390
    uint32_t* p = (uint32_t*)(uintptr_t)r[0]; p[0] = r[1]; p[1] = r[2]; r[4] = 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_30420(uint32_t* r) { r[4] = u32(pA8(r[0]))[0xC / 4]; }   // 1.13c d2common+0x30420
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_303a0(uint32_t* r) { r[4] = pA8(r[0]) ? u32(pA8(r[0]))[0x10 / 4] : 0; }   // 1.13c d2common+0x303A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_30460(uint32_t* r) { r[4] = u32(pA8(r[0]))[8 / 4]; }   // 1.13c d2common+0x30460
// 1.13c D2Common+0x30670 sub_30670: stdcall(unit), player only (unit type 0).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_30670(uint32_t* r) {   // 1.13c d2common+0x30670
    uint32_t* u = (uint32_t*)(uintptr_t)r[0];
    if (u[0] != 0) return;
    uint32_t* pd = (uint32_t*)(uintptr_t)u[0x14 / 4];
    const uint32_t* sk = u32(u32(pA8(r[0]))[8 / 4]);
    pd[0x74 / 4] = (uint32_t)(int32_t)*(const int16_t*)(uintptr_t)sk[0];
    pd[0x7C / 4] = sk[0x34 / 4];
}
