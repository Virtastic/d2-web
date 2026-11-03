// D2CMP 1.13c cell-cache objects in retail's own 32-bit layout.
// The LRU cache (module global D2CMP+0x772B0) is 13 dwords; its node table holds 0x14-byte nodes. Fields
// that retail keeps as pointers are 32-bit guest addresses here, so a captured blob and our object are byte-identical.
#pragma once
#include <cstdint>
#include <cstring>
namespace d2cmp32 {
inline uint32_t ld(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void st(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

struct LruNode {      // 0x14 bytes, LRUCache.cpp
    uint32_t data;    // +00 Fog allocation (or 0)
    uint32_t bytes;   // +04
    uint32_t payload; // +08 owner passed to the destroy callback
    uint32_t next;    // +0C
    uint32_t prev;    // +10
};
struct LruCache {     // 0x34 bytes
    uint32_t identity; // +00 Fog allocation tag
    uint32_t budget;   // +04
    uint32_t used;     // +08
    uint32_t count;    // +0C
    uint32_t nodes;    // +10 node table (capacity * 0x14)
    uint32_t capacity; // +14
    uint32_t head;     // +18 oldest active node
    uint32_t tail;     // +1C
    uint32_t freeHead; // +20
    uint32_t freeTail; // +24
    uint32_t destroy;  // +28 destroy callback (module code; 0xBB60 for the cell cache)
    uint32_t hits;     // +2C
    uint32_t misses;   // +30
};
static_assert(sizeof(LruNode) == 0x14 && sizeof(LruCache) == 0x34, "retail layout");

// The cache's module globals: 6FE872B0 (the object), 6FE87F08 (enabled), 6FE87F0C (budget).
struct Globals { LruCache* cache; uint32_t* enabled; uint32_t* budget; };

// Callees reached through retail's own registers. ctx is the caller's; each hook names the retail callee.
struct Env {
    void* ctx;
    void (*lruInit)(void* ctx, LruCache* c, uint32_t capacity, uint32_t identity, uint32_t budget, uint32_t destroyRva); // 0x17270: EAX capacity, ECX identity, EDX budget, ESI cache, stack destroy
    void (*lruDrain)(void* ctx, LruCache* c);                  // 0x175C0: EDI cache
    void (*lruRelease)(void* ctx, LruCache* c, uint32_t node); // 0x17400: EAX node, EDI cache
    void (*registryShutdown)(void* ctx, uint32_t ecx);         // 0x9880: ECX
    uint32_t (*celBusy)(void* ctx, uint32_t resource);         // 0x8DD8 (ECX resource)
    void (*celRelease)(void* ctx, uint32_t resource);          // 0x8D9C (ECX resource)
};
constexpr uint32_t kDestroyBB60 = 0xBB60;
}
