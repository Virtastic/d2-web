// D2CMP cache: append a node to the cache's LRU tail (ESI = cache, EDI = node in retail).
#include <cstdint>
struct D2CmpCacheNode { uint32_t _0[3]; uint32_t flags; uint32_t tick; D2CmpCacheNode* next; D2CmpCacheNode* prev; };
struct D2CmpCache { uint32_t _0[3]; uint32_t active; uint32_t _a; D2CmpCacheNode* head; D2CmpCacheNode* tail; uint32_t _1[5]; void* lock; uint32_t _2; uint32_t enterCount; };

// 1.13c D2CMP+0x0CAE0
void d2cmp_cae0(D2CmpCache* c, D2CmpCacheNode* n, void (*enter)(void*), void (*leave)(void*), void (*detach)(D2CmpCache*, D2CmpCacheNode*), uint32_t (*tick)())
{
    if (!c->active) return;
    c->enterCount++;
    enter(c->lock);
    // retail: n == NULL -> fatal error string 0xa4a, _exit(-1)
    detach(c, n);
    n->flags &= ~1u;
    if (!c->tail) c->head = n;
    else { c->tail->next = n; n->prev = c->tail; }
    c->tail = n;
    n->tick = tick();
    leave(c->lock);
}
