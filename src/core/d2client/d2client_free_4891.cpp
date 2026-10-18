// src/core/d2client/d2client_free_4891.cpp -- 1.13c D2Client+0x04891 = MSVC 2003 Release CRT _free
// (build-wasm/ghidra-d2client/decomp/0x4891.c). Base 0x6FAB0000; every RVA below is retail's, and the
// module state this file owns is a static named at its retail RVA. The unrecovered CRT callees are host
// seams in retail's call order (the src/core/d2client/crt_chsize_bb34.cpp pattern).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

namespace {
constexpr uint32_t kSehProlog = 0x4904;    // injected __SEH_prolog, scope table 0x6FB7F230
constexpr uint32_t kScopeTable = 0xCF230;  // 0x6FB7F230: pushed by the prolog with 0xC
constexpr uint32_t kLock = 0x4860;         // __lock: push 4; call
constexpr uint32_t kFindBlock = 0x9252;    // ___sbh_find_block(p)
constexpr uint32_t kFreeBlock = 0x927D;    // ___sbh_free_block(region, p)
constexpr uint32_t kUnlock4 = 0x48E4;      // __unlock(4) wrapper
// HeapFree is called through its IAT slot 0x6FB7EF38 (RVA 0xCEF38).
constexpr uint32_t kHeapFree = 0xCEF38;
static_assert(kHeapFree + 0x6FAB0000u == 0x6FB7EF38u, "HeapFree IAT slot");

// Module state, all zero-initialised.
static uint32_t g_heapMode;    // +0x12398C  0x6FBD398C: 3 = the small-block heap is active
static uint32_t g_heapHandle;  // +0x123988  0x6FBD3988: the process heap passed to HeapFree

inline uint32_t go(D2cHost h, uint32_t rva, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) {
    D2cCall c = {};
    c.rva = rva;
    c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    return h(&c);
}
}  // namespace

// 1.13c D2Client+0x04891. cdecl; [esp+4] = p. Undo a NULL free; in heap mode 3 take CRT lock 4, look
// p up in the small-block regions, release the region (which returns it to its bucket and may coalesce
// or VirtualFree), always unlock, and return if the block belonged to a region. Otherwise fall through
// to HeapFree(heap, 0, p). The injected __SEH_epilog only unwinds the frame (it touches no module
// state), so it is stated here and not seamed.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_4891(uint32_t p, D2cHost h) {
    if (p == 0) return;
    go(h, kSehProlog, kScopeTable, 0xC, 0, 0);   // __SEH_prolog(&DAT_6fb7f230, 0xC)
    if (g_heapMode == 3) {
        go(h, kLock, 4, 0, 0, 0);                // __lock(4)
        uint32_t region = go(h, kFindBlock, p, 0, 0, 0);
        if (region != 0) go(h, kFreeBlock, region, p, 0, 0);
        go(h, kUnlock4, 0, 0, 0, 0);             // always, on both find_block outcomes
        if (region != 0) return;
    }
    go(h, kHeapFree, g_heapHandle, 0, p, 0);     // HeapFree(heap, 0, p); the result is ignored
}
