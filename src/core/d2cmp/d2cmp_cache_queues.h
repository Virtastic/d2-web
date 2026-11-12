// The sprite cache's three node queues (1.13c D2CMP 0xBF00 / 0xBF40 / 0xBFB0, d2cmp_cache_runs_wave_a.cpp).
// Retail node: flags +0x0c, stamp +0x10, next +0x14, previous +0x18.
// Pointer members deliberately expand on wasm64/native64; no x86 pointer image
// is reinterpreted as a native object. Queue pairs were +0x14/+0x18,
// +0x1c/+0x20 and +0x24/+0x28 in the retail cache.
#pragma once
#include <cstddef>
#include <cstdint>

struct D2CMPQueueNode {
    uint8_t prefix[12];
    uint32_t flags;
    uint32_t stamp;
    D2CMPQueueNode* next;
    D2CMPQueueNode* previous;
};
struct D2CMPQueuePair { D2CMPQueueNode* head; D2CMPQueueNode* tail; };
struct D2CMPCacheQueues {
    uint8_t prefix[20];
    D2CMPQueuePair normal;
    D2CMPQueuePair alternate;
    D2CMPQueuePair pending;
};
static_assert(offsetof(D2CMPQueueNode, flags) == 12);
static_assert(offsetof(D2CMPQueueNode, stamp) == 16);
static_assert(sizeof(D2CMPQueueNode::next) == sizeof(void*));
static_assert(sizeof(D2CMPQueuePair) == 2 * sizeof(void*));
static_assert(offsetof(D2CMPCacheQueues, alternate) == offsetof(D2CMPCacheQueues, normal) + sizeof(D2CMPQueuePair));
static_assert(offsetof(D2CMPCacheQueues, pending) == offsetof(D2CMPCacheQueues, alternate) + sizeof(D2CMPQueuePair));

extern "C" D2CMPQueueNode* d2cmp_bf00(D2CMPCacheQueues* cache);
extern "C" D2CMPQueueNode* d2cmp_bf40(D2CMPCacheQueues* cache, uint32_t alternate);
extern "C" void d2cmp_bfb0(D2CMPCacheQueues* cache, D2CMPQueueNode* node);
