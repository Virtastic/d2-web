// D2CMP frame-record walk: cache (EAX) + request p (ECX) + data base (EBX). Returns the record address or 0.
#include <cstdint>

// 1.13c D2CMP+0x0d140 (the sub_D140 spec). The p[2] values 2 and 4, and 6 with flag 2 clear, skip the table call;
// every other p[2] goes through tbl12240 (1.13c FUN_12240 on the cache's table index and mask).
extern "C" uint32_t d2cmp_d140(const uint32_t* p, const uint8_t* cache, const uint8_t* base,
                               uint32_t (*tbl12240)(int k, uint32_t mask))
{
    auto rd = [](const uint8_t* q) { uint32_t v; __builtin_memcpy(&v, q, 4); return v; };
    if (base == nullptr || cache == nullptr) return 0;
    if ((uint32_t)cache[4] <= p[0]) return 0;
    uint32_t uVar1 = p[2];
    uint32_t iVar4 = (uint32_t)(uintptr_t)base;
    bool lookup = true;
    if (!(uVar1 == 6 && (p[1] & 2) == 0)) {
        if (uVar1 == 2) { uVar1 = 0; lookup = false; }
        else if (uVar1 == 4) { uVar1 = p[0x10]; lookup = false; }
    }
    if (lookup) uVar1 = tbl12240((int)p[0x10], (uint32_t)(cache[3] - 1));
    if ((int)(cache[3] - 1) <= (int)uVar1) return 0;
    int32_t iVar2 = (int32_t)rd(cache + 0xc + uVar1 * 4);
    if (iVar2 < 0) return 0;
    int32_t iVar3 = 0;
    if ((int32_t)p[0] >= 0) {
        do {
            const uint8_t* e = base + iVar2;
            iVar4 = (uint32_t)(uintptr_t)e;
            if (rd(e) != 0 || 0x100 < rd(e + 4) || 0x100 < rd(e + 8)) return 0;
            iVar2 = iVar2 + 0x23 + (int32_t)rd(e + 0x1c) + (int32_t)rd(e + 0x14);
            if (iVar2 < 0) return 0;
            ++iVar3;
        } while (iVar3 <= (int32_t)p[0]);
    }
    return iVar4;
}
