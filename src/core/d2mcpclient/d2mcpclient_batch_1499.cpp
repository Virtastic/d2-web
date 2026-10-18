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

// +0xA038 DAT_6fa2a038: the CRT lock table, 0x24 slots of 8 bytes (walk ends before 0xA158); dword 0 of
// a slot is the CRITICAL_SECTION pointer, dword 1 the "static lock" flag. Filled by 0x1450
// (__mtinitlocks) and its __mtinitlocknum sibling, so it is zero-initialised here.
static uint32_t g_mcpLockTable[0x24 * 2];

// 1.13c D2MCPClient+0x01499 (__mtdeletelocks): two walks of the lock table. First, every slot holding a
// heap lock (pointer set, flag != 1) is deleted, freed and its pointer cleared. Second, every slot
// holding a static lock (pointer set, flag == 1) is deleted only -- its storage is not heap.
// 1.13c D2MCPClient+0x01499
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2mcpclient_1499(void) {
    uint32_t* table = g_mcpLockTable;
    uint32_t* end = table + 0x24 * 2;
    for (uint32_t* p = table; p < end; p += 2) {
        uint32_t cs = p[0];
        if (cs != 0 && p[1] != 1) {
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)cs);
            std::free((void*)(uintptr_t)cs);
            p[0] = 0;
        }
    }
    for (uint32_t* p = table; p < end; p += 2) {
        if (p[0] != 0 && p[1] == 1) {
            DeleteCriticalSection((LPCRITICAL_SECTION)(uintptr_t)p[0]);
        }
    }
}
