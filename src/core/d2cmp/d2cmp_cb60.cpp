// D2CMP SpriteCache node release. Native pointers replace retail dwords; callees injected.
#include <cstdint>
#include <mutex>

struct CbNode { void* data; uint32_t size; void* payload; uint32_t flags; uint32_t stamp; CbNode *next, *prev; };
struct CbCache {
    uint32_t _0, _4, used, count, _10;
    CbNode *normHead, *normTail, *altHead, *altTail, *pendHead, *pendTail;  // +14..+28
    void (*destroy)(CbNode&);                                               // +2c
    std::mutex* lock;                                                       // +30
};
struct CbHooks {
    void (*unlink)(CbCache&, CbNode&);       // sub_C9F0
    CbNode* (*pop)(CbCache&, uint32_t alt);  // sub_BF40
    void (*pend)(CbCache&, CbNode&);         // sub_BFB0
    void (*release)(void*);                  // Ordinal_10046(SpriteCache.cpp,0xa22)
};

// 1.13c D2CMP+0x0cb60
// EAX = alt, ECX = node (or null: take the queue head), ESI = cache. Returns the released node or null.
CbNode* d2cmp_cb60(CbCache& c, CbNode* n, uint32_t alt, const CbHooks& h) {
    if (!c.count) return nullptr;
    if (n) {
        if (n->flags & 2) return nullptr;
        h.unlink(c, *n);
    } else {
        // Retail: an empty alternate queue or (both queues and alt == 0) takes the normal head inline; otherwise 0xBF40 picks.
        n = c.normHead;
        if (!c.altHead || (n && !alt)) {
            if (!n || (n->flags & 2)) return nullptr;
            CbNode* nx = n->next;
            c.normHead = nx;
            if (!nx) c.normTail = nullptr; else nx->prev = nullptr;
            n->next = n->prev = nullptr;
        } else {
            // Both queues occupied and alt != 0: the alternate queue is taken when its head's stamp is older.
            n = h.pop(c, n ? (uint32_t)(c.altHead->stamp < n->stamp) : alt);
        }
        if (!n || (n->flags & 2)) return nullptr;
    }
    if (!c.lock) return nullptr;
    { std::lock_guard<std::mutex> g(*c.lock); c.used -= n->size; }
    if (n->payload) { c.destroy(*n); n->payload = nullptr; }
    if (n->data) { h.release(n->data); n->data = nullptr; }
    h.pend(c, *n);
    c.count--;
    return n;
}
