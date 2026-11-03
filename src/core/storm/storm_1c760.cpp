// Prior art used: the 1.13c decompile and disassembly; a 1.10f-era Storm reimplementation as a cross-check.
#include "storm_smem_env.h"
using namespace storm_smem;
// 1.13c Storm+0x1C760 SMemReAlloc (ordinal 405), stdcall (ptr, size, tag, line, flags), ret 0x14.
// Cold pool: reported ("SMemReAlloc()", line -1) and ExitProcess(1). A null pointer, a bad magic word (0x8510007C,
// reported when logging is on) or a free block (0x1ACF0 with 0x8510007A) fall through to SMemAlloc(size, tag, line,
// flags). Otherwise the guard and redirect rules of SMemFree apply and 0x1C660 (EAX = header; stack &lockSlot, heap, ptr,
// size, flags) reallocates under the heap lock; the lock slot it leaves behind is the one released.
extern "C" uint32_t Storm13c_1C760(const StormSMemEnv* e, uint32_t ptr, uint32_t size, uint32_t tag, uint32_t line,
                                   uint32_t flags) {
    if (*e->memReady == 0) {
        e->report2F590(0x8510007Du, e->modBase + 0x4765Cu, 0xFFFFFFFFu, 0, 0, 1);
        e->exitProcess(1);
    }
    if (ptr != 0) {
        uint32_t hdr = ptr - 8;
        if (rd16(ptr - 2) == 0x6F6Du) {
            const uint32_t fl = rd8(hdr + 3);
            if (!(fl & 2)) {
                if ((fl & 1) && guardBad(hdr)) e->badBlock1ACF0(0x8510007Bu, tag, line);
                if (rd8(hdr + 3) & 8) hdr = rd32(hdr - 4);
                const uint32_t heap = rd16(hdr + 4) << 16;
                e->enterLock(e->heapLockSlot(heap));
                uint32_t slot = e->heapLockSlot(heap);
                const uint32_t r = e->realloc1C660(hdr, &slot, heap, ptr, size, flags);
                if (slot != 0xFFFFFFFFu) e->leaveLock(slot);
                return r;
            }
            e->badBlock1ACF0(0x8510007Au, tag, line);
        } else {
            *e->lastError = 0x8510007Cu; e->setLastError(0x8510007Cu);
            if (*e->memLog != 0) e->report2F590(0x8510007Cu, tag, line, 0, 1, 1);
        }
    }
    return e->alloc1C580(size, tag, line, flags);
}
