// src/core/d2client/heap_init_8c7f.cpp -- D2Client/__heap_init (RVA 0x08C7F, 81 B), reimplemented from
// the cached decompile build-wasm/ghidra-d2client/decomp/0x8C7F.c. Image base 0x6FAB0000; every RVA and
// global below is retail's. 
//
// Prior art: the same 81-byte VC7.1 `__heap_init` body is already recovered for D2MCPClient 1.13c at
// 0x02357 (src/core/realm/mcp_crt_batch_b56.cpp, d2_mcp_heap_init) and for D2DDraw at 0x2637
// (src/core/gfx/d2ddraw_batch_22ed.cpp). The first argument is the `mtflag` the decompile keeps at
// in_stack_00000004. The heap kind comes from ___heap_select, already recovered here as 0x08C65
//; 0x0920A ___sbh_heap_init is recovered in d2client_batch_77a1.cpp.
// HeapCreate/HeapDestroy are kernel32 imports -- host seams after the retail arguments.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x08C65 (___heap_select): 1 when the platform word is 2 and the major version is >= 5
// (unsigned), else 3. It reads the two words from the env below.
extern "C" uint32_t D2Client_HeapSelect8C65(uint32_t platform, uint32_t major_version);

struct D2ClientHeapInitEnv {
    // host seams
    void*    (*heap_create)(uint32_t options, uint32_t initial_commit, uint32_t maximum_size); // HeapCreate
    int32_t  (*heap_destroy)(void* heap);                          // HeapDestroy
    int32_t  (*sbh_heap_init)(uint32_t threshold);                 // 0x0920A ___sbh_heap_init
    // module state read by ___heap_select (0x08C65)
    uint32_t os_platform;      // +0x0FA8A4  DAT_6fbaa8a4
    uint32_t win_major;        // +0x0FA8B0  DAT_6fbaa8b0
    // module state written here
    void*    heap;             // +0x023988   DAT_6fbd3988: the process heap
    uint32_t mode;             // +0x02398C   DAT_6fbd398c: 3 = small-block heap active
};

// 1.13c D2Client+0x08C7F (81 B, caller 0x04475 __CRT_INIT, process attach): create the process heap
// with HEAP_NO_SERIALIZE when mtflag is 0 (page 0x1000, unbounded), publish the heap kind from
// ___heap_select, and on the small-block path build the 0x3F8-threshold region array. A failed
// HeapCreate returns 0; a failed ___sbh_heap_init destroys the heap and returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_8c7f(int32_t mtflag, D2ClientHeapInitEnv* env) {
    env->heap = env->heap_create(mtflag == 0 ? 1u : 0u, 0x1000, 0);
    if (env->heap == nullptr) return 0;
    env->mode = D2Client_HeapSelect8C65(env->os_platform, env->win_major);
    if (env->mode == 3 && env->sbh_heap_init(0x3f8) == 0) {
        env->heap_destroy(env->heap);
        return 0;
    }
    return 1;
}
