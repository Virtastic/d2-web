// src/core/realm/mcp_heap_term_23a8.cpp -- 1.13c D2MCPClient.dll __heap_term (alpha lane). Base 0x6FA20000.
// Kernel32 calls are host seams after the retail arguments.
#include <cstdint>
#include <emscripten.h>

extern uint32_t g_sbh_heap;  // +0x124A0  DAT_6fa324a0
extern uint32_t g_sbh_mode;  // +0x124A4  DAT_6fa324a4

struct D2McpHeapTermEnv {
    int32_t (*virtual_free)(uint32_t addr, uint32_t size, uint32_t type);  // VirtualFree
    int32_t (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);   // HeapFree
    int32_t (*heap_destroy)(uint32_t heap);                                // HeapDestroy
};

// 1.13c D2MCPClient+0x023A8 (127 B, caller 0x11B8): __heap_term(). Mode 3: for each of `count` (DAT_6fa32250) 20-byte
// region records at `table` (DAT_6fa32254): the u32 at +0x0C is decommitted (0x100000, MEM_DECOMMIT) then released
// (0, MEM_RELEASE), the u32 at +0x10 is HeapFree'd; then the table itself is HeapFree'd. Always HeapDestroy.
// `rec` is the table as u32 words (5 per record).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_heap_term_23a8(int32_t count, uint32_t table, const uint32_t* rec,
                                                           const D2McpHeapTermEnv* env) {
    if (g_sbh_mode == 3) {
        for (int32_t i = 0; i < count; i++) {
            env->virtual_free(rec[i * 5 + 3], 0x100000, 0x4000);
            env->virtual_free(rec[i * 5 + 3], 0, 0x8000);
            env->heap_free(g_sbh_heap, 0, rec[i * 5 + 4]);
        }
        env->heap_free(g_sbh_heap, 0, table);
    }
    env->heap_destroy(g_sbh_heap);
}
