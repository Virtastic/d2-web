// Prior art used: the 1.13c decompile and disassembly; a 1.10f-era Storm reimplementation as a cross-check.
#include "storm_smem_env.h"
using namespace storm_smem;
// 1.13c Storm+0x1A5A0 (ret 4; EAX = header, ECX = &overhead, ESI = &payload, stack user ptr): the block's size split.
static void Storm13c_1A5A0(uint32_t hdr, uint32_t pOverhead, uint32_t pPayload, uint32_t user) {
    if (rd8(hdr + 3) & 4) { wr32(pPayload, rd32(user - 0x10)); wr32(pOverhead, rd8(hdr + 2) + 0x1C); return; }
    uint32_t n = rd8(hdr + 2) + 8;
    if (rd8(hdr + 3) & 1) n += 2;
    wr32(pOverhead, n);
    wr32(pPayload, rd16(hdr) - n);
}
// 1.13c Storm+0x1BAE0 SMemFindNextBlock (ordinal 481), stdcall (heapId, prev, pOut, info), ret 0x10.
// Cold pool: 0x8510007D (reported as "SMemFindNextBlock()" when logging is on); 0. heapId, pOut or info null, or info[0]
// != 0x1C: ERROR_INVALID_PARAMETER (0x57); 0. info[1..6] are zeroed; under the lock of heapId's low byte the heap records
// of that list are visited from the tail back to the head; in each record whose +4 is heapId and whose block range
// [+0x1C, +0x20) is not empty, the blocks are walked by their u16 sizes. The block after the one whose header is prev's
// (prev = 0: the first block met) is the result, the "previous" header carrying over between records. None: *pOut = 0; 0.
// Found: *pOut = info[1] = the user pointer (header+8, or the dword there for a large block), info[2] = !free(header),
// info[3] = !free of the user pointer's own header (0 when its magic is wrong), info[4]/info[5] = 0x1A5A0's payload and
// overhead; 1.
extern "C" uint32_t Storm13c_1BAE0(const StormSMemEnv* e, uint32_t heapId, uint32_t prev, uint32_t pOut, uint32_t info) {
    if (*e->memReady == 0) {
        *e->lastError = 0x8510007Du; e->setLastError(0x8510007Du);
        if (*e->memLog != 0) e->report2F590(0x8510007Du, e->modBase + 0x47734u, 0xFFFFFFFFu, 0, 1, 1);
        return 0;
    }
    if (heapId == 0 || pOut == 0 || info == 0 || rd32(info) != 0x1C) {
        *e->lastError = 0x57; e->setLastError(0x57);
        return 0;
    }
    for (uint32_t k = 1; k <= 6; ++k) wr32(info + 4 * k, 0);
    const uint32_t idx = heapId & 0xFF;
    e->enterLock(idx);
    uint32_t want = 0;
    if (prev != 0) { want = prev - 8; if (rd8(prev - 5) & 8) want = rd32(want - 4); }
    const uint32_t head = e->heapLists[idx];
    uint32_t found = 0, blk = 0, last = 0;
    if (head != 0) {
        uint32_t node = head;
        while (rd32(node) != 0) node = rd32(node);
        for (;;) {
            if (found) break;
            if (rd32(node + 4) == heapId && rd32(node + 0x1C) != rd32(node + 0x20)) {
                const uint32_t end = rd32(node + 0x20);
                blk = rd32(node + 0x1C);
                for (;;) {
                    if (last == want) { found = 1; break; }
                    last = blk; blk += rd16(blk);
                    if (blk == end) break;
                }
            }
            if (node == head) break;
            uint32_t p = head;
            while (rd32(p) != node) p = rd32(p);
            node = p;
        }
    }
    if (!found) { wr32(pOut, 0); e->leaveLock(idx); return 0; }
    uint32_t user = 0;
    if (blk != 0) { user = blk + 8; if (rd8(blk + 3) & 4) user = rd32(user); }
    wr32(pOut, user);
    wr32(info + 4, user);
    wr32(info + 8, ~(rd8(blk + 3) >> 1) & 1);
    wr32(info + 0xC, (user != 0 && rd16(user - 2) == 0x6F6Du) ? (((uint8_t)~rd8(user - 5)) >> 1) & 1 : 0);
    Storm13c_1A5A0(blk, info + 0x14, info + 0x10, user);
    e->leaveLock(idx);
    return 1;
}
