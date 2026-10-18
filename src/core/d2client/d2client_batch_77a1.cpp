// src/core/d2client/d2client_batch_77a1.cpp -- ten 1.13c D2Client.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, batch 7). Base 0x6FAB0000;
// every RVA is retail's, every global this file owns is a static named at its retail RVA.
//
// The batch is the MSVC 2003 Release CRT that the linker pulled into D2Client.dll: the two memory
// primitives (_memmove, _memset), the small-block heap (__sbh_alloc_block/_new_region/_new_group,
// __heap_alloc, calloc, __callnewh) and two _unlock(4) wrappers. The heap owns only the 32-bit retail
// structures; every kernel/heap operation (HeapAlloc/HeapReAlloc/VirtualAlloc/critical section) is a
// host seam in D2ClientBatch77a1Env, after the retail arguments, the d2client_batch_04860.cpp pattern.
// Pointers stored inside the retail structures are 32-bit words, so they are truncated exactly like
// batch 04860 does; wasm64 heap addresses below 4 GiB survive that round trip.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

// _unlock(idx) already recovered in d2client_batch_3664.cpp; retail 0x885D/0x7D2A call it with 4.
extern "C" void d2_client_47ab(uint32_t idx);

struct D2ClientBatch77a1Env {
    void*    (*heap_alloc)(uint32_t size);                    // HeapAlloc(DAT_6fbd3988, 0, size)
    void*    (*heap_alloc_zero)(uint32_t size);               // HeapAlloc(DAT_6fbd3988, 8, size)
    void*    (*heap_realloc)(void* p, uint32_t size);         // HeapReAlloc(DAT_6fbd3988, 0, p, size)
    uint32_t (*heap_free)(void* p);                           // HeapFree(DAT_6fbd3988, 0, p)
    void*    (*virtual_alloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect);
    void     (*lock4)(void);                                  // _lock(4), 0x4860
    void     (*unlock4)(void);                                // _unlock(4), 0x47AB
    uint32_t heap_mode;                                       // 0x2398C DAT_6fbd398c (3 = sbh active)
    uint32_t sbh_threshold;                                   // 0x23978 DAT_6fbd3978
    uint32_t newmode;                                         // 0x0FAAC64 DAT_6fbaac64
    int32_t  (*new_handler)(uint32_t size);                   // 0x0FAAC60 DAT_6fbaac60, may be null
};

// Retail module state, all zero-initialised. Region records are 0x14 bytes: [0]/[1] free-bucket
// bitmaps (buckets 0-31 / 32-63 across all groups), [2] free-group mask, [3] reserved base, [4] group
// table. Region 0's group table is 0x41C4 bytes: current group; 64 bucket counters at +0x04; 32 group
// word0 bitmaps at +0x44; 32 group word1 bitmaps at +0xC4; 32 groups of 64 8-byte list heads at +0x144.
static uint8_t* g_region_list;      // 0x23974 DAT_6fbd3974
static uint32_t g_region_count;     // 0x23970 DAT_6fbd3970
static uint32_t g_region_capacity;  // 0x23980 DAT_6fbd3980
static uint8_t* g_current_region;   // 0x2397C DAT_6fbd397c
static uint8_t* g_region_396c;      // 0x2396C DAT_6fbd396c
static uint32_t g_group_3984;       // 0x23984 DAT_6fbd3984
static uint32_t g_sbh_threshold;    // 0x23978 DAT_6fbd3978 (published by ___sbh_heap_init)

static inline uint32_t ld32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
static inline void st32(void* p, uint32_t v) { std::memcpy(p, &v, 4); }
static inline uint8_t* ptr32(uint32_t v) { return (uint8_t*)(uintptr_t)v; }
static inline uint32_t p32(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" void* d2_client_9595(const D2ClientBatch77a1Env* env);
extern "C" int32_t d2_client_964c(void* region, const D2ClientBatch77a1Env* env);

// 1.13c D2Client+0x031A0 (_memmove): overlap-safe byte copy; a forward pass when dst precedes src,
// otherwise backwards. Result is dst.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_31a0(void* dst, const void* src, uint32_t size) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    if (d == s || size == 0) return dst;
    if ((uintptr_t)d < (uintptr_t)s) {
        for (uint32_t i = 0; i < size; ++i) d[i] = s[i];
    } else {
        for (uint32_t i = size; i != 0; --i) d[i - 1] = s[i - 1];
    }
    return dst;
}

// 1.13c D2Client+0x0A9F0 (_memset): write the low byte of val size times; result is dst.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_a9f0(void* dst, int32_t val, uint32_t size) {
    uint8_t* d = (uint8_t*)dst;
    for (uint32_t i = 0; i < size; ++i) d[i] = (uint8_t)val;
    return dst;
}

// 1.13c D2Client+0x0885D: `_unlock(4)` -- a call to 0x47AB with 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_885d(void) {
    d2_client_47ab(4);
}

// 1.13c D2Client+0x07D2A: `_unlock(4)` -- a call to 0x47AB with 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_7d2a(void) {
    d2_client_47ab(4);
}

// 1.13c D2Client+0x0BA17 (__callnewh): if a new-handler slot is installed, call it with the size; a
// non-zero result means the allocation may be retried.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_ba17(uint32_t size, const D2ClientBatch77a1Env* env) {
    if (env->new_handler != nullptr && env->new_handler(size) != 0) return 1;
    return 0;
}

// 1.13c D2Client+0x09595 (___sbh_alloc_new_region): grow the region array by 0x10 records when full
// (HeapReAlloc, size (capacity*5 + 0x50)*4), then take a zeroed 0x41C4-byte group table and commit a
// 0x100000-byte region with VirtualAlloc(MEM_COMMIT, PAGE_READWRITE). On success publish the record
// ({0, 0, -1, base, groups}, current group -1) and bump the count; a failed VirtualAlloc frees the
// group table. Returns the 0x14-byte record or null.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_9595(const D2ClientBatch77a1Env* env) {
    if (g_region_count == g_region_capacity) {
        void* grown = env->heap_realloc(g_region_list, (g_region_capacity * 5 + 0x50) * 4);
        if (grown == nullptr) return nullptr;
        g_region_capacity += 0x10;
        g_region_list = (uint8_t*)grown;
    }
    uint8_t* rec = g_region_list + (size_t)g_region_count * 0x14;
    void* groups = env->heap_alloc_zero(0x41c4);
    st32(rec + 0x10, p32(groups));
    if (groups == nullptr) return nullptr;
    void* base = env->virtual_alloc(0, 0x100000, 0x2000, 4);
    st32(rec + 0x0c, p32(base));
    if (base == nullptr) {
        env->heap_free(groups);
        return nullptr;
    }
    st32(rec + 0x08, 0xffffffffu);
    st32(rec + 0x00, 0);
    st32(rec + 0x04, 0);
    g_region_count += 1;
    st32(groups, 0xffffffffu);                 // group table current group = -1
    return rec;
}

// 1.13c D2Client+0x0964C (___sbh_alloc_new_group): take the first free group bit of the region's
// group mask, initialise its 63 bucket lists as empty (bucket 63 is the catch-all the loop leaves for
// the block chain below), then commit one 0x8000-byte span at base + group*0x8000. The span is carved
// into eight 0x1000 blocks: size 0xFF0 at +0x0C, next/prev at +0x10/+0x14, previous tail -1 at +0x08,
// tail 0xFF0 at +0xFF8, -1 at +0xFFC. The eight blocks are linked circularly into bucket 63, whose head
// is at group + 0x144 + 0x1F8 ([count unused] next at 0x1F8+4, prev in the stride's spare word at
// 0x200). Sets the group bitmaps (word1 = 1: bucket 63), bumps that bucket's counter, and clears the
// group's bit in the region mask. Returns the group index, or -1 when VirtualAlloc fails.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_964c(void* region, const D2ClientBatch77a1Env* env) {
    uint8_t* rec = (uint8_t*)region;
    uint8_t* groups = (uint8_t*)ptr32(ld32(rec + 0x10));
    uint32_t mask = ld32(rec + 0x08);
    int32_t group = 0;
    for (uint32_t m = mask; (int32_t)m >= 0; m <<= 1) group += 1;
    uint8_t* head = groups + (size_t)group * 0x204 + 0x144;
    int32_t* q = (int32_t*)head;
    for (int32_t left = 0x3f; left != 0; --left) {
        st32(q + 2, p32(q));
        st32(q + 1, p32(q));
        q += 2;
    }
    uint8_t* lpAddress = (uint8_t*)ptr32(ld32(rec + 0x0c) + (uint32_t)group * 0x8000);
    void* committed = env->virtual_alloc(p32(lpAddress), 0x8000, 0x1000, 4);
    if (committed == nullptr) return -1;
    uint8_t* page = lpAddress + 0x10;
    int32_t pages = (int32_t)(0x7000u >> 12) + 1;
    do {
        st32(page - 8, 0xffffffffu);
        st32(page + 0xfec, 0xffffffffu);
        st32(page, p32(page + 0xffc));
        st32(page - 4, 0xff0);
        st32(page + 4, p32(page - 0x1004));
        st32(page + 0xfe8, 0xff0);
        page += 0x1000;
    } while (--pages != 0);
    st32(head + 0x1fc, p32(lpAddress + 0x0c));       // bucket 63 head.next = first block
    st32(lpAddress + 0x14, p32(head + 0x1f8));       // first block.prev = bucket 63 head
    st32(head + 0x200, p32(lpAddress + 0x700c));     // bucket 63 head.prev = last block
    st32(lpAddress + 0x7010, p32(head + 0x1f8));     // last block.next = bucket 63 head
    st32(groups + 0x44 + group * 4, 0);
    st32(groups + 0xc4 + group * 4, 1);
    uint8_t* counter = groups + 0x43;
    uint8_t was = *counter;
    *counter = (uint8_t)(was + 1);
    if (was == 0) st32(rec + 0x04, ld32(rec + 0x04) | 1);
    st32(rec + 0x08, ld32(rec + 0x08) & ~(0x80000000u >> ((uint8_t)group & 0x1f)));
    return group;
}

// 1.13c D2Client+0x09A31 (___sbh_alloc_block): round the request up to a 16-byte footprint with a
// 0x17 bias, derive the first acceptable bucket, and search the region array for a region whose
// bitmaps can serve it -- from the current region to the end, then from the start to the current
// region, then any region with a free group, allocating a region/group only when all are exhausted.
// The chosen group is the first with a non-empty masked bitmap; the bucket is its highest set bit.
// The free block is carved from its END: the allocated head sentinel (size+1) and tail sentinel go
// there, the remainder (if any) keeps the front and is relinked into its own bucket, maintaining the
// bucket counter and the group/region bits when a list crosses empty. Returns payload = sentinel + 4.
// The pointer is 32-bit like retail's. Pre-init current==0 is read as the array start, the state
// ___sbh_heap_init leaves.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_9a31(uint32_t size, const D2ClientBatch77a1Env* env) {
    uint32_t footprint = (size + 0x17u) & 0xfffffff0u;
    int32_t bucket = (int32_t)((size + 0x17u) >> 4) - 1;
    uint8_t bucket_byte = (uint8_t)bucket;
    uint32_t word0_mask, word1_mask;
    if (bucket < 0x20) {
        word0_mask = 0xffffffffu >> (bucket_byte & 0x1f);
        word1_mask = 0xffffffffu;
    } else {
        word0_mask = 0;
        word1_mask = 0xffffffffu >> ((uint8_t)(bucket_byte - 0x20) & 0x1f);
    }

    uint8_t* end = g_region_list + (size_t)g_region_count * 0x14;
    uint8_t* cur = g_current_region != nullptr ? g_current_region : g_region_list;
    uint8_t* p = cur;
    while ((uintptr_t)p < (uintptr_t)end &&
           (ld32(p + 4) & word1_mask) == 0 && (ld32(p) & word0_mask) == 0)
        p += 0x14;
    uint8_t* q = g_region_list;
    if (p == end) {
        while ((uintptr_t)q < (uintptr_t)cur &&
               (ld32(q + 4) & word1_mask) == 0 && (ld32(q) & word0_mask) == 0)
            q += 0x14;
        p = q;
        if (q == cur) {
            uint8_t* r = q;
            while ((uintptr_t)r < (uintptr_t)end && ld32(r + 8) == 0) r += 0x14;
            uint8_t* s = g_region_list;
            p = r;
            if (r == end) {
                while ((uintptr_t)s < (uintptr_t)cur && ld32(s + 8) == 0) s += 0x14;
                p = s;
                if (s == cur) {
                    p = (uint8_t*)d2_client_9595(env);
                    if (p == nullptr) return nullptr;
                }
            }
            int32_t made = d2_client_964c(p, env);
            st32(ptr32(ld32(p + 0x10)), (uint32_t)made);
            if ((int32_t)ld32(ptr32(ld32(p + 0x10))) == -1) return nullptr;
        }
    }

    uint8_t* groups = (uint8_t*)ptr32(ld32(p + 0x10));
    int32_t group = (int32_t)ld32(groups);
    if (group == -1 ||
        ((ld32(groups + 0x31 * 4 + group * 4) & word1_mask) == 0 &&
         (ld32(groups + 0x11 * 4 + group * 4) & word0_mask) == 0)) {
        group = 0;
        uint8_t* w = groups + 0x11 * 4;
        if ((ld32(groups + 0x31 * 4) & word1_mask) == 0 && (ld32(w) & word0_mask) == 0) {
            uint8_t* w2;
            do {
                w2 = w + 0x21 * 4;
                group += 1;
                w += 4;
            } while ((ld32(w2) & word1_mask) == 0 && (ld32(w) & word0_mask) == 0);
        }
    }

    uint8_t* head = groups + (size_t)group * 0x81 * 4 + 0x51 * 4;
    int32_t b = 0;
    uint32_t bits = ld32(groups + 0x11 * 4 + group * 4) & word0_mask;
    if (bits == 0) {
        bits = ld32(groups + 0x31 * 4 + group * 4) & word1_mask;
        b = 0x20;
    }
    while ((int32_t)bits >= 0) {
        b += 1;
        bits <<= 1;
    }

    int32_t* node = (int32_t*)ptr32(ld32(head + (size_t)b * 8 + 4));
    int32_t leftover = *node - (int32_t)footprint;
    int32_t new_bucket = (leftover >> 4) - 1;
    if (new_bucket > 0x3f) new_bucket = 0x3f;
    g_current_region = p;

    if (new_bucket != b) {
        if (node[1] == node[2]) {                    // the only node in its list
            if (b < 0x20) {
                uint32_t bit = 0x80000000u >> ((uint8_t)b & 0x1f);
                st32(groups + 0x11 * 4 + group * 4, ld32(groups + 0x11 * 4 + group * 4) & ~bit);
                uint8_t* counter = groups + b + 4;
                *counter = (uint8_t)(*counter - 1);
                if (*counter == 0) st32(p, ld32(p) & ~bit);
            } else {
                uint32_t bit = 0x80000000u >> ((uint8_t)(b - 0x20) & 0x1f);
                st32(groups + 0x31 * 4 + group * 4, ld32(groups + 0x31 * 4 + group * 4) & ~bit);
                uint8_t* counter = groups + b + 4;
                *counter = (uint8_t)(*counter - 1);
                if (*counter == 0) st32(p + 4, ld32(p + 4) & ~bit);
            }
        }
        st32(ptr32((uint32_t)node[2]) + 4, (uint32_t)node[1]);   // prev->next = next
        st32(ptr32((uint32_t)node[1]) + 8, (uint32_t)node[2]);   // next->prev = prev
        if (leftover != 0) {
            uint8_t* nb = head + (size_t)new_bucket * 8;
            int32_t first = (int32_t)ld32(nb + 4);
            node[2] = (int32_t)p32(nb);
            node[1] = first;
            st32(nb + 4, (uint32_t)(uintptr_t)node);
            st32(ptr32((uint32_t)node[1]) + 8, (uint32_t)(uintptr_t)node);
            if (node[1] == node[2]) {                // list was empty: counter and bits
                uint8_t* counter = groups + new_bucket + 4;
                uint8_t was = *counter;
                *counter = (uint8_t)(was + 1);
                if (new_bucket < 0x20) {
                    uint32_t bit = 0x80000000u >> ((uint8_t)new_bucket & 0x1f);
                    if (was == 0) st32(p, ld32(p) | bit);
                    st32(groups + 0x11 * 4 + group * 4, ld32(groups + 0x11 * 4 + group * 4) | bit);
                } else {
                    uint32_t bit = 0x80000000u >> ((uint8_t)(new_bucket - 0x20) & 0x1f);
                    if (was == 0) st32(p + 4, ld32(p + 4) | bit);
                    st32(groups + 0x31 * 4 + group * 4, ld32(groups + 0x31 * 4 + group * 4) | bit);
                }
            }
        }
    }
    if (leftover != 0) {
        *node = leftover;
        st32((uint8_t*)node + leftover - 4, (uint32_t)leftover);
    }
    uint8_t* block = (uint8_t*)node + leftover;
    st32(block, footprint + 1);
    st32(block + footprint - 4, footprint + 1);
    int32_t count = (int32_t)ld32(head);
    st32(head, (uint32_t)(count + 1));
    if (count == 0 && p == g_region_396c && group == (int32_t)g_group_3984) g_region_396c = nullptr;
    st32(groups, (uint32_t)group);
    return block + 4;
}

// 1.13c D2Client+0x07CB8 (__heap_alloc): small-block path first when the heap is in mode 3 and the
// size is under the sbh threshold (lock 4 around ___sbh_alloc_block); otherwise HeapAlloc, with 0
// mapped to 1 byte and the size rounded up to 16 unless the heap is in mode 1.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_7cb8(uint32_t size, const D2ClientBatch77a1Env* env) {
    if (env->heap_mode == 3 && size <= env->sbh_threshold) {
        env->lock4();
        void* p = d2_client_9a31(size, env);
        env->unlock4();
        if (p != nullptr) return p;
    }
    uint32_t bytes = size == 0 ? 1 : size;
    if (env->heap_mode != 1) bytes = (bytes + 0x0f) & 0xfffffff0u;
    return env->heap_alloc(bytes);
}

// 1.13c D2Client+0x08797 (calloc over the CRT heap): reject n*size > 0xFFFFFFFF-0x1F via the
// 0xffffffe0/n guard. A zero product is a one-byte request. Small blocks (heap mode 3 + size within
// the threshold) are rounded to 16 and taken from ___sbh_alloc_block under lock 4, then zeroed with
// _memset; everything else is HeapAlloc with HEAP_ZERO_MEMORY. On failure the new-handler is asked to
// make room (when __newmode is set) and the whole attempt repeats.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_8797(uint32_t count, uint32_t size, const D2ClientBatch77a1Env* env) {
    if (count != 0 && size > 0xffffffe0u / count) return nullptr;
    uint32_t product = count * size;
    uint32_t bytes = product == 0 ? 1 : product;
    for (;;) {
        if (bytes < 0xffffffe1u) {
            if (env->heap_mode == 3) {
                bytes = (bytes + 0x0f) & 0xfffffff0u;
                if (product <= env->sbh_threshold) {
                    env->lock4();
                    void* p = d2_client_9a31(product, env);
                    env->unlock4();
                    if (p != nullptr) {
                        d2_client_a9f0(p, 0, product);
                        return p;
                    }
                }
            }
            void* p = env->heap_alloc_zero(bytes);
            if (p != nullptr) return p;
        }
        if (env->newmode == 0) return nullptr;
        if (d2_client_ba17(bytes, env) == 0) return nullptr;
    }
}

// 1.13c D2Client+0x0920A (___sbh_heap_init): take the 0x140-byte region array from the big-block heap
// (HeapAlloc(DAT_6fbd3988, 0, 0x140)); on success zero the region list and count, point the current
// region at the array, publish the small-block threshold and the 0x10-record capacity. Returns 1, or 0
// when the allocation fails (retail leaves the globals untouched then).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_920a(uint32_t threshold, const D2ClientBatch77a1Env* env) {
    void* regions = env->heap_alloc(0x140);
    if (regions == nullptr) return 0;
    g_region_list = (uint8_t*)regions;
    g_region_396c = nullptr;
    g_region_count = 0;
    g_current_region = (uint8_t*)regions;
    g_sbh_threshold = threshold;
    g_region_capacity = 0x10;
    return 1;
}
