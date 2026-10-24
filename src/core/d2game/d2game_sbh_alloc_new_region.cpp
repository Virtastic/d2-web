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
#include <cstdlib>
#include <cstring>

struct D2GameSbh { uint32_t count, capacity, regions, heap, last_region, entry, seq; };
extern "C" { D2GameSbh g_D2Game_sbh; }
// Win32 boundary (libc-backed): HEAP_ZERO_MEMORY=8; fresh VirtualAlloc ranges are zeroed 64K-aligned.
extern "C" uint32_t D2Game_HeapReAlloc(uint32_t, uint32_t, uint32_t p, uint32_t n) { return (uint32_t)(uintptr_t)realloc((void*)(uintptr_t)p, n); }
extern "C" uint32_t D2Game_HeapAlloc(uint32_t, uint32_t f, uint32_t n) { void* p = malloc(n); if (p && (f & 8)) memset(p, 0, n); return (uint32_t)(uintptr_t)p; }
extern "C" void D2Game_HeapFree(uint32_t, uint32_t, uint32_t p) { free((void*)(uintptr_t)p); }
extern "C" void* D2Game_VirtualAlloc(void* addr, uint32_t n, uint32_t, uint32_t) {
    if (addr) return addr;
    void* p = aligned_alloc(0x10000, n);
    if (p) memset(p, 0, n);
    return p;
}

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }

// 1.13c D2Game+0x08F60 (183 B, caller 0x093FC ___sbh_alloc_block): ___sbh_alloc_new_region. When the
// region table is full, grow it by 0x10 entries ((capacity*5 + 0x50) dwords). Take the next 0x14-byte
// region record, allocate its 0x41C4-byte zeroed control block and reserve its 1 MB range
// (VirtualAlloc(0, 0x100000, MEM_RESERVE, PAGE_READWRITE)). On success clear the record, bump the count
// and mark the control block as having no current group (0xFFFFFFFF). A failed VirtualAlloc frees the
// control block; either allocation failure returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2game_sbh_alloc_new_region() {
    D2GameSbh* env = &g_D2Game_sbh;
    if (env->count == env->capacity) {
        uint32_t grown = D2Game_HeapReAlloc(env->heap, 0, env->regions,
                                          (env->capacity * 5u + 0x50u) * 4u);
        if (grown == 0) return 0;
        env->capacity += 0x10;
        env->regions = grown;
    }
    uint32_t region = env->regions + env->count * 0x14u;
    uint32_t control = D2Game_HeapAlloc(env->heap, 8, 0x41C4);
    P32(region + 0x10)[0] = control;
    if (control != 0) {
        uint32_t base = (uint32_t)(uintptr_t)D2Game_VirtualAlloc(nullptr, 0x100000, 0x2000, 4);
        P32(region + 0xC)[0] = base;
        if (base != 0) {
            P32(region + 8)[0] = 0xFFFFFFFFu;
            P32(region)[0] = 0;
            P32(region + 4)[0] = 0;
            env->count += 1;
            P32(control)[0] = 0xFFFFFFFFu;
            return region;
        }
        D2Game_HeapFree(env->heap, 0, control);
    }
    return 0;
}
