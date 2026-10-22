// src/core/d2mcpclient/d2mcpclient_batch_1499.cpp -- 1.13c D2MCPClient.dll row reimplemented from the
// cached decompile (build-wasm/ghidra-d2mcpclient/decomp/0x1499.c). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector. Retail base 0x6FA20000.
//
// Host model follows the same CRT-lock-table row already implemented for D2DDraw
// (src/core/gfx/d2ddraw_batch2_rowex.cpp, __mtdeletelocks at 0x19C2): the table is module state, the
// kernels are host seams under wasm, and the CRT heap free is the libc heap.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>
#include <Windows.h>   // DeleteCriticalSection: host (kernel) state, a no-op under wasm

// +0xA038: the CRT lock table filled by 0x1450 (__mtinitlocks); one table, defined there.
struct D2McpLockSlot { uintptr_t cs; uint32_t init; uint32_t pad; };
extern D2McpLockSlot g_lockTable[0x24];

// 1.13c D2MCPClient+0x01499 (__mtdeletelocks): two walks of the lock table. First, every slot holding a
// heap lock (pointer set, flag != 1) is deleted, freed and its pointer cleared. Second, every slot
// holding a static lock (pointer set, flag == 1) is deleted only -- its storage is not heap.
// 1.13c D2MCPClient+0x01499
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2mcpclient_1499(void) {
    D2McpLockSlot* end = g_lockTable + 0x24;
    for (D2McpLockSlot* p = g_lockTable; p < end; ++p) {
        if (p->cs != 0 && p->init != 1) {
            DeleteCriticalSection((LPCRITICAL_SECTION)p->cs);
            std::free((void*)p->cs);
            p->cs = 0;
        }
    }
    for (D2McpLockSlot* p = g_lockTable; p < end; ++p) {
        if (p->cs != 0 && p->init == 1) DeleteCriticalSection((LPCRITICAL_SECTION)p->cs);
    }
}
