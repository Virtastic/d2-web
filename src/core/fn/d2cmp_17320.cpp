// d2cmp+0x17320 sub_17320: FUNCTION-mode probe.
#include <emscripten.h>
#include <cstdint>
#include "../d2cmp/d2cmp_tile_lru113c.h"

// r[0] = node (EAX), r[1] = cache (EDX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_17320(uint32_t* r) {
    d2cmp_17320((D2CmpLru113c*)(uintptr_t)r[1], (D2CmpLruNode113c*)(uintptr_t)r[0]);
    r[4] = 0;
}
