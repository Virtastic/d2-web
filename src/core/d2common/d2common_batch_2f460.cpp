// D2Common 1.13c batch. Unit-like record: +0 type (must be 0), +0x14 sub-pointer, sub+0x90 returned.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x2F460: return field 0x90 of the record at p+0x14.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_2f460(const int32_t* p) {
 if (!p || p[0] != 0 || p[5] == 0) __builtin_trap(); // retail: error report 0xfb4/0xfb6/0x14d7, then _exit(-1)
 return *reinterpret_cast<const uint32_t*>(static_cast<intptr_t>(static_cast<uint32_t>(p[5])) + 0x90);
}
}
