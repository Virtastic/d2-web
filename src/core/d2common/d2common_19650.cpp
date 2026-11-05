// 1.13c D2Common+0x19650: the sibling token-table pair (name strings at module 0x95084 and 0x917C8, alloc line 0x37, fatal 0x38). EAX = archive handle; void.
// Publishes into the module dwords 0xA13C4 (total), 0xA13C8 / 0xA13CC (pool), 0xA13D0 (second table).
#include <emscripten.h>
#include "d2common_tokpair.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_seam_d2common_19650(uint32_t arch, const D2CommonTokPairEnv* E, uint32_t* out4) {
    const D2CommonTokPairOut o = d2common_tokpair(arch, 0x95084, 0x917c8, 0x37, E);
    out4[0] = o.total; out4[1] = o.pool; out4[2] = o.pool2; out4[3] = o.second;   // globals 13C4 13C8 13CC 13D0
}
