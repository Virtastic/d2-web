// storm+0x5440 FUNCTION-mode probe: regs eax ebx; r[0] = n (EAX), r[1] = table T (EBX).
// Blobs: lg 0 (r[14]), O 1 (r[16], old block), F 2 (r[18], fresh block returned by the 0x1C580 hook), T 3 (r[20]).
#include <emscripten.h>
#include <cstdint>
#include "storm_hk.h"
extern "C" void Storm_Sub5440(void* tbl, uint32_t n);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_5440(uint32_t* r) {
    const uint32_t sc[] = { 0x1C580, r[18], 0, 0 };
    stormhk::Scope hk(r, 0, 4, sc);
    Storm_Sub5440((void*)(uintptr_t)r[1], r[0]);
    r[4] = 0;
}
