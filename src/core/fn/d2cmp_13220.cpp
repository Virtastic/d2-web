// d2cmp+0x13220 DccDirectionBox: FUNCTION-mode probe.
#include <emscripten.h>
#include <cstdint>
#include "../d2cmp/d2cmp_dcc.h"

// r[0] = frames (EAX), r[1] = hdr (ECX), r[2] = dir (EDX), r[3] = box (EDI)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_13220(uint32_t* r) {
    d2cmp_13220((const int32_t*)(uintptr_t)r[1], (int32_t)r[2],
                (DccFrame*)(uintptr_t)r[0], (DccBox*)(uintptr_t)r[3], nullptr);
    r[4] = 0;
}
