// 1.13c D2Common+0x84110:preset-record walk. EBX = pCtx, stack pUnit. Table = [pCtx+0x50]
// (+0x4C count, +0x50 records of 0x18 bytes). An empty table does nothing. Otherwise a unit room type (+0x14) of -1 becomes
// 0x3E; the start index is the seed roll on the owner's seed pair ([pUnit]+0x1C4, range = count) unless pCtx+0x44 is set
// (start 0); records (start+i) mod count (signed) go to the worker 0x83E20 (unit, ctx, record), and an accepted record ends
// the walk while pCtx+0x44 is clear. The roll and the worker are the caller's seams.
#include <cstdint>
#include <cstring>

static uint32_t Rd(uintptr_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(a), 4); return v; }

extern "C" void D2Common_84110_Walk(uintptr_t pUnit, uintptr_t pCtx, uint32_t (*pfRoll)(uintptr_t, int32_t),
                                    int32_t (*pfWork)(uintptr_t, uintptr_t, uintptr_t))
{
    const uintptr_t pTable = Rd(pCtx + 0x50);
    const int32_t nCount = static_cast<int32_t>(Rd(pTable + 0x4C));
    if (nCount == 0) return;
    if (static_cast<int32_t>(Rd(pUnit + 0x14)) == -1)
    {
        const uint32_t v = 0x3E;
        memcpy(reinterpret_cast<void*>(pUnit + 0x14), &v, 4);
    }
    int32_t nStart = 0;
    if (Rd(pCtx + 0x44) == 0) nStart = static_cast<int32_t>(pfRoll(static_cast<uint32_t>(Rd(pUnit) + 0x1C4u), nCount));
    for (int32_t i = 0; i < nCount; ++i)
    {
        const int32_t nIndex = static_cast<int32_t>(static_cast<uint32_t>(nStart) + static_cast<uint32_t>(i)) % nCount;
        const uintptr_t pRecord = static_cast<uint32_t>(Rd(pTable + 0x50) + static_cast<uint32_t>(nIndex) * 0x18u);
        if (pfWork(pUnit, pCtx, pRecord) != 0 && Rd(pCtx + 0x44) == 0) return;
    }
}
