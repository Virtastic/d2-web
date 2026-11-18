#include <cstdint>
#include <cstring>
// Retail 32-bit layout: the record, the list link and the list head are guest dwords; the allocator (Fog #10042 / #10043), the stream
// initialiser (0x7090) and the lock are host seams. The globals are reached through pointers (tag = 0x6F9C6584, avail = 0x6F9C6590, head = 0x6F9C65CC).
struct D2sound7740Env {
    uint32_t (*alloc)(uint32_t size, uint32_t line);
    void (*release)(uint32_t p, uint32_t line);
    uint32_t (*init7090)(uint32_t rec, uint32_t flags);
    void (*lock)(void);
    void (*unlock)(void);
    uint32_t* tag;
    uint32_t avail;
    uint32_t* head;
};
// 1.13c D2sound+0x7740 allocate a 0x60-byte sound record, init via 0x7090, link at the head of the global list.
uint32_t D2sound_7740(uint32_t flags, uint32_t arg, const D2sound7740Env* e) {
    *e->tag = arg;
    if (((flags & 2) && !e->avail) || ((flags & 2) && (flags & 1))) return 0;
    const uint32_t rec = e->alloc(0x60, 0x1d9);
    std::memset((void*)(uintptr_t)rec, 0, 0x60);
    if (!e->init7090(rec, flags)) { e->release(rec, 0x1e0); return 0; }
    e->lock();
    const uint32_t link = e->alloc(8, 0xd4);
    uint32_t w[2] = { rec, *e->head };
    std::memcpy((void*)(uintptr_t)link, w, 8);
    *e->head = link;
    e->unlock();
    return rec;
}
