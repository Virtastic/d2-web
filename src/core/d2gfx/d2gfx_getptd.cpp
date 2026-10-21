// src/core/d2gfx/d2gfx_getptd.cpp -- 1.13c D2Gfx CRT __getptd, reimplemented from the cached decompile
// (ghidra-d2gfx/decomp/0x28FE.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec,
// no vector. Host model follows d2gfx_crt_helpers.cpp: callees whose own rows are UNMAPPED are seams on a
// caller-supplied struct in retail argument order; module globals are statics named by their retail RVA.
#include <cstdint>

// 0x2C45 calloc, the two kernel32 imports reached through the IAT slots, 0x18EB __amsg_exit (terminal).
struct D2GfxGetptdHost {
    uint32_t (*get_last_error)(void);
    void     (*set_last_error)(uint32_t err);
    void*    (*tls_get_value)(uint32_t index);
    int      (*tls_set_value)(uint32_t index, void* value);
    void*    (*calloc_)(uint32_t count, uint32_t size);
    uint32_t (*get_current_thread_id)(void);
    void     (*amsg_exit)(int32_t code);
};

extern "C" uint32_t d2gfx_ptd_tls_index;
uint32_t d2gfx_ptd_tls_index;   // DAT_6fa90170 (RVA 0x10170), the TlsAlloc index

// 1.13c D2Gfx+0x028FE __getptd (113 bytes): _ptiddata __cdecl __getptd(void).
// GetLastError is saved, TlsGetValue(DAT_6fa90170) returns the calling thread's block or NULL. On a miss,
// calloc(1, 0x8C) allocates the ptd and TlsSetValue stores it; on store success the block is seeded with
// _initaddr +0x54 = &DAT_6fa901a0 (RVA 0x101A0, the port's module-relative form), _holdrand +0x14 = 1,
// _tid +0x00 = GetCurrentThreadId() and _thandle +0x04 = -1. An allocation or store failure runs
// __amsg_exit(0x10) (terminal in retail; on a host that returns, the decompile's fall-through path is
// taken). The saved error is restored before returning the block, NULL only in the failed case.
// 1.13c D2Gfx+0x028FE
extern "C"
void* d2gfx_getptd(const D2GfxGetptdHost* host) {
    uint32_t saved = host->get_last_error();
    uint8_t* ptd = (uint8_t*)host->tls_get_value(d2gfx_ptd_tls_index);
    if (ptd == nullptr) {
        ptd = (uint8_t*)host->calloc_(1, 0x8C);
        if (ptd != nullptr) {
            if (host->tls_set_value(d2gfx_ptd_tls_index, ptd) != 0) {
                *(uint32_t*)(ptd + 0x54) = 0x101A0u;        // _initaddr, retail 0x6FA901A0
                *(uint32_t*)(ptd + 0x14) = 1;               // _holdrand
                uint32_t tid = host->get_current_thread_id();
                *(uint32_t*)(ptd + 0x04) = 0xFFFFFFFFu;     // _thandle
                *(uint32_t*)(ptd + 0x00) = tid;             // _tid
                goto done;
            }
        }
        host->amsg_exit(0x10);
    }
done:
    host->set_last_error(saved);
    return ptd;
}
