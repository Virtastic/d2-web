// d2cmp+0x134d0 DccBitRead: FUNCTION-mode probe.
#include <emscripten.h>
#include <cstdint>
#include "../d2cmp/d2cmp_dcc.h"

// r[0] = n (EAX), r[1] = DccBits* (EDX); r[4] = result
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_134d0(uint32_t* r) {
    r[4] = d2cmp_134d0((int32_t)r[0], (DccBits*)(uintptr_t)r[1]);
}
