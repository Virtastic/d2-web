// 1.13c D2Common+0x19190: objtype/objmode token tables (name strings at module 0x94FD4 and 0x94FCC, alloc line 0x94). EAX = archive handle; void.
// Publishes total/pool/pool/second into the module dwords 0x1213B4 / 0x1213B8 / 0x1213BC / 0x1213C0.
#include <emscripten.h>
#include "d2common_tokpair.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_seam_d2common_19190(uint32_t arch, const D2CommonTokPairEnv* E, uint32_t* out4) {
    const D2CommonTokPairOut o = d2common_tokpair(arch, 0x94fd4, 0x94fcc, 0x94, E);
    out4[0] = o.total; out4[1] = o.pool; out4[2] = o.pool2; out4[3] = o.second;   // globals 13B4 13B8 13BC 13C0
}
