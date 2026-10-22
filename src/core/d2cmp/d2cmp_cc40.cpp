// D2CMP SpriteCache node acquire (evicting). Native pointers replace retail dwords; callees injected.
#include <cstdint>
#include <cstddef>
#include <mutex>

struct CcNode { void* data; uint32_t size; void* payload; uint32_t flags; uint32_t stamp; CcNode* next; CcNode* prev; };
struct CcCache {
    uint32_t _0, limit, used, count, _10;
    CcNode *aHead, *aTail, *bHead, *bTail, *freeHead, *freeTail;  // +14..+28
    void (*destroy)(CcNode&);                                      // +2c
    std::mutex* lock;                                              // +30
    uint32_t total, live;                                          // +34, +38
};
struct CcHooks {
    CcNode* (*getFree)(CcCache&, uint32_t alt);       // sub_C030
    CcNode* (*popQueue)(CcCache&, uint32_t alt);      // sub_BF40
    void* (*alloc)();                                 // Ordinal_10045(SpriteCache.cpp,0x99d)
    void (*release)(void*);                           // Ordinal_10046(SpriteCache.cpp,0xa22)
    uint32_t (*tick)();                               // GetTickCount
    void (*fatal)();                                  // Ordinal_10265(0x971)+10024+_exit
};

// 1.13c D2CMP+0x0CC40
// EAX = cache; args (key, unused, alt). The stamp dword lands in node+8 (retail quirk, kept).
CcNode* d2cmp_cc40(CcCache& c, uint32_t key, uint32_t alt, const CcHooks& h) {
    if (key >= c.limit) h.fatal();
    uint32_t stamp = 0;
    if (alt) {
        if (c.aHead) stamp = c.aHead->stamp;
        if (c.bHead && c.bHead->stamp <= stamp) stamp = c.bHead->stamp;
    }
    for (;;) {
        if (CcNode* n = h.getFree(c, alt)) {
            n->flags = alt ? (n->flags | 1) : (n->flags & ~1u);
            n->data = h.alloc();
            n->size = key;
            n->payload = (void*)(uintptr_t)stamp;
            c.live++; c.total++;
            return n;
        }
        if ((alt && h.tick() - stamp < 15000) || c.count == 0) return nullptr;
        CcNode* v;
        auto take = [&](CcNode*& head, CcNode*& tail) {
            v = head;
            if (!v || (v->flags & 2)) return false;
            head = v->next;
            (head ? head->prev : tail) = nullptr;
            v->next = v->prev = nullptr;
            return true;
        };
        if (!c.bHead) { if (!take(c.aHead, c.aTail)) return nullptr; }
        else if (!c.aHead) { if (!take(c.bHead, c.bTail)) return nullptr; }
        else v = h.popQueue(c, c.bHead->stamp);
        if (!v || (v->flags & 2) || !c.lock) return nullptr;
        { std::lock_guard<std::mutex> g(*c.lock); c.used -= v->size; }
        if (v->payload) { c.destroy(*v); v->payload = nullptr; }
        if (v->data) { h.release(v->data); v->data = nullptr; }
        if (!c.freeTail) c.freeHead = v; else { c.freeTail->next = v; v->prev = c.freeTail; }
        c.freeTail = v;
        c.count--;
    }
}
