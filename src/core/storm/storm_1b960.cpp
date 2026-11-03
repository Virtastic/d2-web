// Prior art used: the 1.13c decompile and disassembly; a 1.10f-era Storm reimplementation as a cross-check.
#include "storm_smem_env.h"
using namespace storm_smem;
// 1.13c Storm+0x1B960 SMemFree (ordinal 403), stdcall (ptr, tag, line, flags), ret 0x10.
// Cold pool: error 0x8510007D, reported as "SMemFree()" line -1 when logging is on; 0. Null pointer: 0x85100081, reported
// with the caller's tag and line; 0. Magic word at ptr-2 not 0x6F6D: 0x8510007C, reported; 0. Free flag (2): 0x1ACF0 with
// 0x8510007A; 0. Guard flag (1) with a bad guard word: 0x1ACF0 with 0x8510007B, then freed anyway. Redirect flag (8): the
// header is the dword before it. The heap (header u16 at +4, high word) is locked around 0x1B170 (EDX = header,
// stack heap, ptr); 1.
extern "C" uint32_t Storm13c_1B960(const StormSMemEnv* e, uint32_t ptr, uint32_t tag, uint32_t line, uint32_t flags) {
    (void)flags;
    if (*e->memReady == 0) {
        *e->lastError = 0x8510007Du; e->setLastError(0x8510007Du);
        if (*e->memLog != 0) e->report2F590(0x8510007Du, e->modBase + 0x47714u, 0xFFFFFFFFu, 0, 1, 1);
        return 0;
    }
    if (ptr == 0) {
        *e->lastError = 0x85100081u; e->setLastError(0x85100081u);
        if (*e->memLog != 0) e->report2F590(0x85100081u, tag, line, 0, 1, 1);
        return 0;
    }
    uint32_t hdr = ptr - 8;
    if (rd16(ptr - 2) != 0x6F6Du) {
        *e->lastError = 0x8510007Cu; e->setLastError(0x8510007Cu);
        if (*e->memLog != 0) e->report2F590(0x8510007Cu, tag, line, 0, 1, 1);
        return 0;
    }
    const uint32_t fl = rd8(hdr + 3);
    if (fl & 2) { e->badBlock1ACF0(0x8510007Au, tag, line); return 0; }
    if ((fl & 1) && guardBad(hdr)) e->badBlock1ACF0(0x8510007Bu, tag, line);
    if (rd8(hdr + 3) & 8) hdr = rd32(hdr - 4);
    const uint32_t heap = rd16(hdr + 4) << 16;
    e->enterLock(e->heapLockSlot(heap));
    const uint32_t slot = e->heapLockSlot(heap);
    e->free1B170(hdr, heap, ptr);
    if (slot != 0xFFFFFFFFu) e->leaveLock(slot);
    return 1;
}
