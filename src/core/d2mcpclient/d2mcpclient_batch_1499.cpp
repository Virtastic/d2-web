// src/core/d2mcpclient/d2mcpclient_batch_1499.cpp -- 1.13c D2MCPClient.dll row reimplemented from the
// cached decompile (build-wasm/ghidra-d2mcpclient/decomp/0x1499.c). Retail base 0x6FA20000.
//
// Host model follows the same CRT-lock-table row already implemented for D2DDraw
// (src/core/gfx/d2ddraw_batch2_rowex.cpp, __mtdeletelocks at 0x19C2): the table is module state, the
// kernel DeleteCriticalSection and the CRT heap free are seams (the host's by default).
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>
#include <Windows.h>   // DeleteCriticalSection: host (kernel) state, a no-op under wasm

// +0xA038: the CRT lock table filled by 0x1450 (__mtinitlocks); one table, defined there.
struct D2McpLockSlot { uintptr_t cs; uint32_t init; uint32_t pad; };
extern D2McpLockSlot g_lockTable[0x24];

struct D2McpDeleteLocksSeams {
    uint32_t (*deleteCS)(uintptr_t cs);   // kernel32 DeleteCriticalSection through EBX; its EAX is what retail leaves
    void (*free_)(uintptr_t p);           // the CRT _free at 0x1BB4
};

// D2MCPClient 0x01499 (1.13c) (__mtdeletelocks): two walks of the lock table. First, every slot holding a
// heap lock (pointer set, flag != 1) is deleted, freed and its pointer cleared. Second, every slot
// holding a static lock (pointer set, flag == 1) is deleted only -- its storage is not heap. A lock shared
// by two slots is deleted and freed once per slot, as retail does. Returns the EAX retail leaves at `ret`:
// the second walk loads each slot's pointer into EAX, so it is the last slot's pointer (0 after the first
// walk cleared it), or DeleteCriticalSection's EAX when the last slot is a set static lock.
// 1.13c D2MCPClient+0x01499
extern "C" uint32_t d2_mcp_mtdeletelocks_with(const D2McpDeleteLocksSeams* s, D2McpLockSlot* t) {
    D2McpLockSlot* end = t + 0x24;
    for (D2McpLockSlot* p = t; p < end; ++p) {
        if (p->cs != 0 && p->init != 1) {
            s->deleteCS(p->cs);
            s->free_(p->cs);
            p->cs = 0;
        }
    }
    uint32_t eax = 0;
    for (D2McpLockSlot* p = t; p < end; ++p) {
        eax = (uint32_t)p->cs;
        if (p->cs != 0 && p->init == 1) eax = s->deleteCS(p->cs);
    }
    return eax;
}

static uint32_t hostDeleteCS(uintptr_t cs) { DeleteCriticalSection((LPCRITICAL_SECTION)cs); return 0; }
static void hostFree(uintptr_t p) { std::free((void*)p); }

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_mtdeletelocks(void) {
    static const D2McpDeleteLocksSeams S = { hostDeleteCS, hostFree };
    d2_mcp_mtdeletelocks_with(&S, g_lockTable);
}
