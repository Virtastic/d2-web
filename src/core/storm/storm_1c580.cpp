// Prior art used: the 1.13c decompile and disassembly; a 1.10f-era Storm reimplementation as a cross-check.
#include "storm_smem_env.h"
using namespace storm_smem;
// 1.13c Storm+0x1C580 SMemAlloc (ordinal 401), stdcall (size, tag, line, flags), ret 0x10.
// Cold pool: reported ("SMemAlloc()", line -1) and ExitProcess(1). The heap id comes from 0x1AD80 (ESI = tag, EBX = line);
// its low byte picks the list and the lock. Under that lock the list is searched for the record whose +4 is the id; a miss
// creates one with 0x1BDD0 (EAX = tag; stack line, id, idx, 0x1000, 0x1000, 0x10000). 0x1C390 (EAX = heap, EBX = &lockSlot;
// stack flags, size) allocates; the lock slot it leaves is released. A tracked heap (Storm+0x55160) other than this one
// is checked with 0x1A600. Returns 0x1C390's block.
extern "C" uint32_t Storm13c_1C580(const StormSMemEnv* e, uint32_t size, uint32_t tag, uint32_t line, uint32_t flags) {
    if (*e->memReady == 0) {
        e->report2F590(0x8510007Du, e->modBase + 0x47780u, 0xFFFFFFFFu, 0, 0, 1);
        e->exitProcess(1);
    }
    const uint32_t id = e->heapId1AD80(tag, line);
    const uint32_t idx = id & 0xFF;
    e->enterLock(idx);
    uint32_t slot = idx;
    uint32_t heap = e->heapLists[idx];
    while (heap != 0 && rd32(heap + 4) != id) heap = rd32(heap);
    if (heap == 0) heap = e->newHeap1BDD0(tag, line, id, idx, 0x1000, 0x1000, 0x10000);
    const uint32_t r = e->alloc1C390(heap, &slot, flags, size);
    if (slot != 0xFFFFFFFFu) e->leaveLock(slot);
    if (*e->heapTrack != 0 && *e->heapTrack != heap) e->check1A600();
    return r;
}
