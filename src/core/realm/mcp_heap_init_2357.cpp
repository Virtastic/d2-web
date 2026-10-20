// src/core/realm/mcp_heap_init_2357.cpp -- 1.13c D2MCPClient.dll __heap_init (alpha lane). Base 0x6FA20000.
// Kernel32 HeapCreate/HeapDestroy and the sibling CRT calls are host seams after the retail argument.
#include <cstdint>
#include <emscripten.h>

extern uint32_t g_sbh_heap;  // +0x124A0  DAT_6fa324a0
uint32_t g_sbh_mode;         // +0x124A4  DAT_6fa324a4

struct D2McpHeapInitEnv {
    uint32_t (*heap_create)(uint32_t flags, uint32_t initial, uint32_t maximum);  // HeapCreate
    int32_t  (*heap_destroy)(uint32_t heap);                                      // HeapDestroy
    int32_t  (*heap_select)();                                                    // 0x233D ___heap_select
    int32_t  (*sbh_heap_init)(uint32_t threshold);                                // 0x34FC
};

// 1.13c D2MCPClient+0x02357 (81 B, caller 0x11B8): __heap_init(mtflag). HeapCreate(mtflag == 0 ? HEAP_NO_SERIALIZE
// : 0, 0x1000, 0); null -> 0. Mode = ___heap_select(); mode 3 needs ___sbh_heap_init(0x3F8), else the heap is
// destroyed and 0 returned. Success 1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_heap_init_2357(int32_t mtflag, const D2McpHeapInitEnv* env) {
    g_sbh_heap = env->heap_create(mtflag == 0, 0x1000, 0);
    if (g_sbh_heap == 0) return 0;
    g_sbh_mode = (uint32_t)env->heap_select();
    if (g_sbh_mode == 3 && env->sbh_heap_init(0x3f8) == 0) {
        env->heap_destroy(g_sbh_heap);
        return 0;
    }
    return 1;
}
