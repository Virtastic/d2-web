// src/core/d2game/d2game_sbh_alloc_new_region.cpp -- 1.13c D2Game.dll row 0x08F60,
// ___sbh_alloc_new_region, from the cached decompile (build-wasm/ghidra-d2game/decomp/0x8F60.c).
//
// Prior art: BNClient 0x4D54 (src/core/realm/bnclient_batch_4d54.cpp), D2MCPClient 0x3887
// (src/core/realm/mcp_sbh_alloc.cpp) and D2Client 0x9595 (src/core/d2client/d2client_batch_77a1.cpp)
// port the same MSVC 2003 (VC7.1) Release CRT function byte for byte; the rule is identical here.
//
// Host model: the four retail globals are host-owned pointers/values in the env (as in
// src/core/d2game/d2game_batch_c2150.cpp): 0x6FD39FB8 region count, 0x6FD39FC8 table capacity,
// 0x6FD39FBC region array base, 0x6FD3A20C heap handle. The Win32 calls are seams.
#include <cstdint>
#include <emscripten.h>

struct D2GameSbhNewRegionEnv {
    uint32_t (*heapReAlloc)(uint32_t heap, uint32_t flags, uint32_t block, uint32_t size);
    uint32_t (*heapAlloc)(uint32_t heap, uint32_t flags, uint32_t size);
    uint32_t (*virtualAlloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect);
    int32_t  (*heapFree)(uint32_t heap, uint32_t flags, uint32_t block);       // BOOL
    uint32_t* count;        // 0x6FD39FB8 DAT_6fd39fb8: region count
    uint32_t* capacity;     // 0x6FD39FC8 DAT_6fd39fc8: region-array capacity
    uint32_t* regions;      // 0x6FD39FBC DAT_6fd39fbc: region-array base
    uint32_t  heap;         // 0x6FD3A20C DAT_6fd3a20c: Win32 heap handle
};

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }

// 1.13c D2Game+0x08F60 (183 B, caller 0x093FC ___sbh_alloc_block): ___sbh_alloc_new_region. When the
// region table is full, grow it by 0x10 entries ((capacity*5 + 0x50) dwords). Take the next 0x14-byte
// region record, allocate its 0x41C4-byte zeroed control block and reserve its 1 MB range
// (VirtualAlloc(0, 0x100000, MEM_RESERVE, PAGE_READWRITE)). On success clear the record, bump the count
// and mark the control block as having no current group (0xFFFFFFFF). A failed VirtualAlloc frees the
// control block; either allocation failure returns 0.
// 1.13c D2Game+0x08F60
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_8F60(const D2GameSbhNewRegionEnv* env) {
    if (*env->count == *env->capacity) {
        uint32_t grown = env->heapReAlloc(env->heap, 0, *env->regions,
                                          (*env->capacity * 5u + 0x50u) * 4u);
        if (grown == 0) return 0;
        *env->capacity += 0x10;
        *env->regions = grown;
    }
    uint32_t region = *env->regions + *env->count * 0x14u;
    uint32_t control = env->heapAlloc(env->heap, 8, 0x41C4);
    P32(region + 0x10)[0] = control;
    if (control != 0) {
        uint32_t base = env->virtualAlloc(0, 0x100000, 0x2000, 4);
        P32(region + 0xC)[0] = base;
        if (base != 0) {
            P32(region + 8)[0] = 0xFFFFFFFFu;
            P32(region)[0] = 0;
            P32(region + 4)[0] = 0;
            *env->count += 1;
            P32(control)[0] = 0xFFFFFFFFu;
            return region;
        }
        env->heapFree(env->heap, 0, control);
    }
    return 0;
}
