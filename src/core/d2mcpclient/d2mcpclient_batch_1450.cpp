// src/core/d2mcpclient/d2mcpclient_batch_1450.cpp -- 1.13c D2MCPClient.dll row reimplemented from the
// cached decompile (build-wasm/ghidra-d2mcpclient/decomp/0x1450.c). Alpha batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector. Host model follows
// src/core/d2game/d2game_batch_3e70.cpp: the module globals are statics at the retail RVA named below,
// and the one callee whose row is not ours (0x27AD) is a seam on a caller-supplied struct.
#include <cstdint>

// RVA 0xA038: _locktable, 0x24 entries of 8 bytes. The slot itself is the dword pair; the decompile
// reads dword [i*2] (the section pointer) and dword [i*2+1] (the image's "initialise me" flag).
struct D2McpLockSlot { uintptr_t cs; uint32_t init; uint32_t pad; };
static D2McpLockSlot g_lockTable[0x24];

// RVA 0xAA40: the section arena the initialiser hands out in 0x18-byte steps, one per flagged slot.
static uint8_t g_csArena[0x24 * 0x18];

// 0x27AD ___crtInitCritSecAndSpinCount, whose own row has no engine implementation.
struct D2McpBatchLockHost {
    int32_t (*initCritSecAndSpinCount)(void* cs, uint32_t spin);
};

// 1.13c D2MCPClient+0x1450 (__mtinitlocks): walk all 0x24 lock slots. A slot whose flag dword is 1 is
// pointed at the next 0x18-byte arena block and that block is initialised with spin 4000; a failed init
// clears the slot and returns 0. Slots whose flag is not 1 are left untouched. Returns 1 after the walk.
// 1.13c D2MCPClient+0x1450
extern "C"
int32_t d2_fn_d2mcpclient_1450(const D2McpBatchLockHost* host) {
    uint8_t* arena = g_csArena;
    for (int32_t i = 0; i < 0x24; ++i) {
        if (g_lockTable[i].init == 1) {
            g_lockTable[i].cs = (uintptr_t)arena;
            arena += 0x18;
            if (host->initCritSecAndSpinCount((void*)g_lockTable[i].cs, 4000) == 0) {
                g_lockTable[i].cs = 0;
                return 0;
            }
        }
    }
    return 1;
}
