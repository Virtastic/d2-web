// Prior art used: D2MOO D2CMP/include/D2CMP.h tile-entry layout (1.10f).
// D2MOO's D2CMP implementation is absent; the 1.13c cached decompiles
// establish the getter offsets and LRU mutation order. Stage A only.
#include "d2cmp_tile_lru113c.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace {
struct TilePrefix {
    uint32_t light;
    uint16_t roof, flags;
    uint32_t totalHeight, width, bottom, type, style, sequence, rarity;
};
static_assert(offsetof(TilePrefix, flags) == 6);
static_assert(offsetof(TilePrefix, totalHeight) == 8);
static_assert(offsetof(TilePrefix, rarity) == 0x20);
struct RetailLruNode { uint32_t data, size, callbackValue, next, previous; };
static_assert(offsetof(RetailLruNode, next) == 0xc);
static_assert(offsetof(RetailLruNode, previous) == 0x10);
static_assert(sizeof(RetailLruNode) == 0x14);
// Retail reports an assertion through Fog and terminates with -1. There is
// no recovery path; native termination preserves that control-flow contract.
[[noreturn]] void fatal() { std::_Exit(-1); }
template<class T> T field(const void* p, size_t offset) {
    if (!p) fatal();
    T value;
    std::memcpy(&value, static_cast<const unsigned char*>(p) + offset, sizeof value);
    return value;
}
}
// 1.13c D2CMP+0x15210
extern "C" uint32_t d2cmp_15210(const void* tile) { return field<uint32_t>(tile, 8); }
// 1.13c D2CMP+0x152A0
extern "C" uint32_t d2cmp_152a0(const void* tile) { return field<uint32_t>(tile, 0x20); }
// 1.13c D2CMP+0x15300
extern "C" uint16_t d2cmp_15300(const void* tile) { return field<uint16_t>(tile, 6); }

// 1.13c D2CMP+0x17170
extern "C" D2CmpLruNode113c* d2cmp_17170(D2CmpLru113c* cache) {
    auto* node = cache->first;
    if (!node) return nullptr;
    cache->first = node->next;
    if (cache->first) cache->first->previous = nullptr;
    else cache->last = nullptr;
    node->next = nullptr;
    node->previous = nullptr;
    return node;
}
// 1.13c D2CMP+0x17320
extern "C" void d2cmp_17320(D2CmpLru113c* cache, D2CmpLruNode113c* node) {
    if (!cache->first) return;
    if (!node) fatal();
    if (node == cache->first) { d2cmp_17170(cache); return; }
    if (node == cache->last) {
        cache->last = node->previous;
        cache->last->next = nullptr;
    } else {
        node->previous->next = node->next;
        node->next->previous = node->previous;
    }
    node->next = nullptr;
    node->previous = nullptr;
}
// 1.13c D2CMP+0x173A0
extern "C" void d2cmp_173a0(D2CmpLru113c* cache, D2CmpLruNode113c* node) {
    if (!cache->activeCount) return;
    ++cache->touches;
    if (!node) fatal();
    d2cmp_17320(cache, node);
    if (!cache->last) {
        cache->first = node;
        cache->last = node;
        return;
    }
    cache->last->next = node;
    node->previous = cache->last;
    cache->last = node;
}
