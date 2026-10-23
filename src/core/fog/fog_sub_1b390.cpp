// 1.13c Fog+0x1B390: allocate a new pool block (header + usage bitmap + VirtualAlloc'd data), register it.
// Retail 32-bit layout; callees and the fatal path are seams.
#include <cstdint>
#include <cstdlib>

struct fog_1b390_api {
    void* (*realloc_)(void* p, uint32_t size);            // Fog+0x20D7
    void* (*malloc_)(uint32_t size);                      // Fog+0x20C5
    void* (*valloc)(uint32_t size);                       // VirtualAlloc(0,size,MEM_COMMIT,PAGE_READWRITE)
    void (*insert)(uint8_t* alloc, uint32_t* blk);        // Fog+0x1B240 (EAX=alloc)
    void (*fatal)(int line, const char* msg);             // 1AD20, FC20, ord116, 10F30(6,"",line,..), 184A0, _exit(-1)
};

uint32_t* fog_sub_1b390(const fog_1b390_api* a, uint8_t* alloc, uint8_t* pool) {  // 1.13c Fog+0x1B390
    int32_t& cnt = *(int32_t*)(alloc + 0x7a0);
    int32_t& cap = *(int32_t*)(alloc + 0x7a4);
    void*& tab = *(void**)(alloc + 0x7a8);
    if (cap <= cnt) {
        cap += 0x20;
        tab = a->realloc_(tab, (uint32_t)cap * 8);
        if (!tab) a->fatal(0x2ff, "(1) Pool Blocks overflowed at %d");
    }
    uint32_t words = (*(uint32_t*)(pool + 0x1c) + 0x1f) >> 5;
    uint32_t* b = (uint32_t*)a->malloc_(words * 4 + 0x18);
    void* mem = a->valloc(*(uint32_t*)(pool + 0x20));
    if (!b || !mem) { a->fatal(800, "(2) nPoolBlocks = %d/%d  nBlockSize = %d  nUsageTableSize = %d"); return nullptr; }
    for (int i = 0; i < 6; i++) b[i] = 0;
    b[1] = (uint32_t)(uintptr_t)(b + 6);
    for (uint32_t i = 0; i < words; i++) b[6 + i] = 0;
    b[5] = (uint32_t)(uintptr_t)pool;
    b[0] = (uint32_t)(uintptr_t)mem;
    (*(int32_t*)(pool + 0x24))++;
    a->insert(alloc, b);
    return b;
}
