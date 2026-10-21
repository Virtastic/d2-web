// D2Common 1.13c batch: stat-list callback dispatch. Callee register args are not in the decompile;
// both take (stat list, id) here and are weak externs.
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) int32_t d2_fn_d2common_38160(uint32_t list, uint32_t id);
__attribute__((weak)) int32_t d2_fn_d2common_382b0(uint32_t list, uint32_t id);

// 1.13c D2Common+0x388E0
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_388e0(intptr_t p1, uint32_t p2, uint32_t p3, void (*cb)(uint32_t, uint32_t)) {
 (void)p3;
 if (!p1) return;
 const uint32_t a = *reinterpret_cast<const uint32_t*>(p1 + 0x5c);
 if (!a) return;
 const intptr_t s = (intptr_t)a;
 if (*reinterpret_cast<const int32_t*>(s + 0x10) >= 0) return;
 if (d2_fn_d2common_38160(a, p2) != -1) return;
 const int32_t i = d2_fn_d2common_382b0(a, p2);
 if (i < 0) return;
 const intptr_t e = (intptr_t)*reinterpret_cast<const uint32_t*>(s + 0x24) + i * 8;
 if (e) cb(*reinterpret_cast<const uint32_t*>(e + 4), p2);
}
}
