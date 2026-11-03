// 1.13c D2CMP+0xBC00: release every cached record of a cel file object. stdcall(obj).
#include "d2cmp_cache32.h"
using namespace d2cmp32;
// obj+0x10C count, obj+0x110 records (0x60 bytes each); a record's +0x5C is its LRU node, +0x54 its data.
// 1.13c D2CMP+0xBC00
extern "C" void d2cmp_bc00(uint8_t* obj, LruCache* cache, const Env& e) {
    int32_t i = 0, ebx = 0;
    uint32_t count;
    std::memcpy(&count, obj + 0x10C, 4);
    if ((int32_t)count <= 0) return;
    do {
        uint32_t recs; std::memcpy(&recs, obj + 0x110, 4);
        uint32_t rec = recs + (uint32_t)ebx;
        if (ld(rec + 0x5C)) {
            e.lruRelease(e.ctx, cache, ld(rec + 0x5C));
            st(rec + 0x54, 0);
            st(rec + 0x5C, 0);
        }
        std::memcpy(&count, obj + 0x10C, 4);   // re-read every pass
        ++i; ebx += 0x60;
    } while (i < (int32_t)count);
}
