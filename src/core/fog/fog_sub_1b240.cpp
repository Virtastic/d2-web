// src/core/fog/fog_sub_1b240.cpp -- Fog+0x1B240, register a pool block in the manager's sorted block table.
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct FogPoolBlockInfo { uint8_t pad[0x1c]; int32_t bits; int32_t dataSize; };  // +0x1c, +0x20
struct FogPoolBlock { uint32_t key; uint32_t pad[4]; FogPoolBlockInfo* info; };  // key at +0, info at +0x14
struct FogPoolEntry { FogPoolBlock* block; uint32_t key; };
struct FogPoolMgr {  // EAX on entry; critical section (+4) is a no-op on the host
    int32_t count;      // +0x7a0
    int32_t capacity;   // +0x7a4
    FogPoolEntry* tab;  // +0x7a8
    int32_t totalBytes; // +0x17a8
};

// 1.13c Fog+0x1B240
// Custom convention: EAX=manager, stack param_1=block. Inserts at the lower-bound of key in the
// key-sorted table (growing it by 0x20 entries when full; realloc failure is fatal) and adds
// dataSize + 0x18 + ((bits+0x1f)>>5)*4 to totalBytes.
void fog_sub_1B240(FogPoolMgr* m, FogPoolBlock* b) {
    if (m->capacity <= m->count) {
        m->capacity += 0x20;
        m->tab = (FogPoolEntry*)realloc(m->tab, (size_t)m->capacity * sizeof(FogPoolEntry));
        if (!m->tab) abort();  // retail: log "(1) Pool Blocks overflowed", error 0x261, _exit(-1)
    }
    int n = m->count;
    m->totalBytes += b->info->dataSize + 0x18 + ((b->info->bits + 0x1f) >> 5) * 4;
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = lo + ((hi - lo) >> 1);
        if (m->tab[mid].key < b->key) lo = mid + 1; else hi = mid;
    }
    if (lo < n) memmove(&m->tab[lo + 1], &m->tab[lo], (size_t)(n - lo) * sizeof(FogPoolEntry));
    m->tab[lo] = {b, b->key};
    m->count++;
}
