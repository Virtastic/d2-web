// src/core/realm/bnclient_batch_4d54.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x4D54.c (image base 0x6FF20000).
//
// Row: ___sbh_alloc_new_region 0x4D54, MSVC 2003 (VC7.1) Release CRT small-block heap. Same rule as
// D2MCPClient 0x3887 (src/core/realm/mcp_sbh_alloc.cpp); the BN module's globals (DAT_6FF40BC8 count,
// BD8 capacity, BCC region array, BE0 heap) are passed in. Heap/VirtualAlloc calls are seams.
#include <cstdint>
#include <emscripten.h>

struct BnSbhNewRegionEnv {
    uint32_t (*heap_realloc)(uint32_t heap, uint32_t flags, uint32_t block, uint32_t size);
    uint32_t (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t size);
    uint32_t (*virtual_alloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect);
    int32_t  (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);   // BOOL
    uint32_t* count;        // DAT_6FF40BC8: region count
    uint32_t* capacity;     // DAT_6FF40BD8: region-array capacity
    uint32_t* regions;      // DAT_6FF40BCC: region-array base
    uint32_t* heap;         // DAT_6FF40BE0: Win32 heap handle
};

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }

// 1.13c BNClient+0x04D54 (183 B, caller 0x51F0 ___sbh_alloc_block): ___sbh_alloc_new_region. When the
// region table is full, grow it by 0x10 entries ((capacity*5 + 0x50) dwords). Carve the next 0x14-byte
// region record, allocate its 0x41C4-byte zeroed control block, and reserve its 1 MB range
// (MEM_RESERVE, PAGE_READWRITE). On success clear the record, bump the count and mark the control
// block as having no current group (0xFFFFFFFF); on either allocation failure free what was taken and
// return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_sbh_alloc_new_region(const BnSbhNewRegionEnv* env) {
    if (*env->count == *env->capacity) {
        uint32_t grown = env->heap_realloc(*env->heap, 0, *env->regions,
                                           (*env->capacity * 5u + 0x50u) * 4u);
        if (grown == 0) return 0;
        *env->capacity += 0x10;
        *env->regions = grown;
    }
    uint32_t region = *env->regions + *env->count * 0x14u;
    uint32_t control = env->heap_alloc(*env->heap, 8, 0x41C4);
    P32(region + 0x10)[0] = control;
    if (control != 0) {
        uint32_t base = env->virtual_alloc(0, 0x100000, 0x2000, 4);
        P32(region + 0xC)[0] = base;
        if (base != 0) {
            P32(region + 8)[0] = 0xffffffffu;
            P32(region)[0] = 0;
            P32(region + 4)[0] = 0;
            *env->count += 1;
            P32(control)[0] = 0xffffffffu;
            return region;
        }
        env->heap_free(*env->heap, 0, control);
    }
    return 0;
}
