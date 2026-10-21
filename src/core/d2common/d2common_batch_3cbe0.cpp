// D2Common 1.13c batch. Unwired retail callees are weak so the file links.
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W uint32_t Ordinal_10265(uint32_t);
W void Ordinal_10024(const void*, uint32_t);
W void sub_7BF20(void*, int32_t, int32_t);                                 // 0x7BF20
W char DAT_6fdda728[1];

// 1.13c D2Common+0x3CBE0: null owner is fatal; else forward (owner+0x48, p3, p4) to sub_7BF20.
EMSCRIPTEN_KEEPALIVE void sub_3CBE0(intptr_t owner, uint32_t, int32_t p3, int32_t p4) {
 if (!owner) { Ordinal_10024(DAT_6fdda728, Ordinal_10265(0x2e9)); __builtin_trap(); }
 sub_7BF20(reinterpret_cast<void*>(static_cast<intptr_t>(*reinterpret_cast<const uint32_t*>(owner + 0x48))), p3, p4);
}
}
