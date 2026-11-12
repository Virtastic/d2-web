// d2cmp+0x17400 sub_17400: FUNCTION-mode probe. The blobs hold the retail 32-bit cache and nodes; the probe imports them into
// the port's native structs (d2cmp_lrucache_probe.h), calls src/core/d2cmp/d2cmp_cache_lifecycle.cpp:d2cmp_17400, writes them back.
// No tuple holds a node allocation or payload, so the release and destroy callbacks are unreachable (abort).
#include <emscripten.h>
#include <cstdint>
#include <cstdlib>
#include "d2cmp_lrucache_probe.h"
extern "C" void d2cmp_17400(D2CMPLruCache& c, D2CMPLruNode* n, const D2CMPCacheMemory& memory);

namespace {
void* noAlloc(uint32_t, size_t, const char*, uint32_t) { abort(); }
void noRelease(uint32_t, void*, const char*, uint32_t) { abort(); }
void noDestroy(D2CMPLruNode&) { abort(); }
}

// r[0] = memory (ECX), r[1] = node or 0 (EAX), r[2] = cache (EDI)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_17400(uint32_t* r) {
    lrup::World w{}; w.importCache(r[2]); w.c.destroyPayload = noDestroy;
    const D2CMPCacheMemory mem{noAlloc, noRelease};
    d2cmp_17400(w.c, w.node(r[1]), mem);
    w.exportNodes(); w.exportCache(r[2]);
    r[4] = 0;
}
