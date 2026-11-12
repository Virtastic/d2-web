// d2cmp+0x171e0 sub_171E0: FUNCTION-mode probe. The blobs hold the retail 32-bit cache and nodes; the probe imports them into
// the port's native structs (d2cmp_lrucache_probe.h), calls src/core/d2cmp/d2cmp_cache_lifecycle.cpp:d2cmp_171e0, writes them back.
#include <emscripten.h>
#include <cstdint>
#include "d2cmp_lrucache_probe.h"
extern "C" D2CMPLruNode* d2cmp_171e0(D2CMPLruCache& c, uint32_t size);

// r[0] = cache (ECX), r[1] = size delta (EDI); r[4] = node or 0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_171e0(uint32_t* r) {
    lrup::World w{}; w.importCache(r[0]);
    D2CMPLruNode* n = d2cmp_171e0(w.c, r[1]);
    r[4] = w.addrOf(n);
    w.exportNodes(); w.exportCache(r[0]);
}
