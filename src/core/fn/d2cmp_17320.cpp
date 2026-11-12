// d2cmp+0x17320 sub_17320: FUNCTION-mode probe. r[0] = node (EAX), r[1] = cache (EDX); blobs 0..2 are the nodes n1 n2 n3,
// blob 3 the cache. The port works on native nodes (8-byte links), so the probe lifts the retail 32-bit links (node +0xc next,
// +0x10 previous; cache +0x18 first, +0x1c last) into native nodes, runs the port, and writes the links back as blob addresses.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "../d2cmp/d2cmp_tile_lru113c.h"

namespace {
uint32_t rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void wr(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_17320(uint32_t* r) {
    const uint32_t addr[3] = {r[14], r[16], r[18]};
    D2CmpLruNode113c nodes[3];
    auto toNode = [&](uint32_t a) -> D2CmpLruNode113c* {
        for (int k = 0; k < 3; ++k) if (a && a == addr[k]) return &nodes[k];
        return nullptr;
    };
    auto toAddr = [&](const D2CmpLruNode113c* n) -> uint32_t { return n ? addr[n - nodes] : 0; };
    for (int k = 0; k < 3; ++k) {
        nodes[k].size = rd(addr[k] + 4);
        nodes[k].callbackValue = rd(addr[k] + 8);
        nodes[k].next = toNode(rd(addr[k] + 0xc));
        nodes[k].previous = toNode(rd(addr[k] + 0x10));
    }
    const uint32_t c = r[20];
    D2CmpLru113c cache;
    cache.activeCount = rd(c + 0xc);
    cache.first = toNode(rd(c + 0x18));
    cache.last = toNode(rd(c + 0x1c));
    cache.touches = rd(c + 0x30);
    d2cmp_17320(&cache, toNode(r[0]));
    for (int k = 0; k < 3; ++k) {
        wr(addr[k] + 0xc, toAddr(nodes[k].next));
        wr(addr[k] + 0x10, toAddr(nodes[k].previous));
    }
    wr(c + 0x18, toAddr(cache.first));
    wr(c + 0x1c, toAddr(cache.last));
    r[4] = 0;
}
