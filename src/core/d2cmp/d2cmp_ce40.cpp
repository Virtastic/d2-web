// D2CMP SpriteCache teardown. Native pointers replace retail dwords.
#include <cstdint>
#include <mutex>

struct CeNode { void* data; uint32_t size; void* payload; uint32_t flags; uint32_t key; CeNode* next; CeNode* prev; };
struct CeCache {
    uint32_t _0, _4, used, count;
    void* buf;                         // +10
    CeNode *aHead, *aTail, *bHead, *bTail, *freeHead, *freeTail;  // +14..+28
    void (*destroy)(CeNode&);          // +2c
    std::mutex* lock;                  // +30
};

// 1.13c D2CMP+0xCE40
// lookup = sub_BF40(key, cache); release = Ordinal_10046 / Ordinal_10043 (free).
void d2cmp_ce40(CeCache& c, CeNode* (*lookup)(uint32_t, CeCache&), void (*release)(void*)) {
    while (c.count) {
        CeNode* n = c.bHead;
        auto unlink = [&](CeNode*& head, CeNode*& tail) {
            if (!n || (n->flags & 2)) return false;
            head = n->next;
            (head ? head->prev : tail) = nullptr;
            n->next = n->prev = nullptr;
            return true;
        };
        if (!n) { n = c.aHead; if (!unlink(c.aHead, c.aTail)) break; }
        else if (!c.aHead) { if (!unlink(c.bHead, c.bTail)) break; }
        else n = lookup(n->key, c);
        if (!n || (n->flags & 2) || !c.lock) break;
        { std::lock_guard<std::mutex> g(*c.lock); c.used -= n->size; }
        if (n->payload) { c.destroy(*n); n->payload = nullptr; }
        if (n->data) { release(n->data); n->data = nullptr; }
        if (c.freeTail) { c.freeTail->next = n; n->prev = c.freeTail; } else c.freeHead = n;
        c.freeTail = n;
        if (--c.count == 0) break;
    }
    if (c.buf) { release(c.buf); c.buf = nullptr; }
    if (c.lock) { release(c.lock); c.lock = nullptr; }
}
