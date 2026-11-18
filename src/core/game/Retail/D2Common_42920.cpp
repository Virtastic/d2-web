// Round-robin node finder of the level fixup placer, moved from the stand-alone d2common batch 7ecc0 into the game tree.
// Retail 32-bit layout: the list links are 4-byte host addresses.
#include <cstdint>
#include <cstring>

namespace {
template <class T> T rd(const void* p, int o) { T v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
template <class T> void wr(void* p, int o, T v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }
}

// 1.13c D2Common+0x42920: find-or-create a per-id round-robin node on the list at +0x1cc of pCtx
// (node: id, count, cursor, next), seed LCG at +0x1c4/+0x1c8, then advance cursor = (cursor+1)%count.
// pRow = the 0x1b0-byte table row (id at +0, count at +0x40); alloc = Ordinal_10045.
int32_t D2Common_42920(void* pCtx, int32_t id, const void* pRow, int32_t* (*alloc)())
{
    // list links are 4-byte retail pointers (wasm32 host addresses), not native pointer width
    int32_t* n = (int32_t*)(uintptr_t)rd<uint32_t>(pCtx, 0x1cc);
    for (; n; n = (int32_t*)(uintptr_t)rd<uint32_t>(n, 12)) if (n[0] == id) goto found;
    {
        n = alloc();
        n[0] = rd<int32_t>(pRow, 0); n[1] = rd<int32_t>(pRow, 0x40); n[2] = 0; n[3] = 0;
        uint32_t c = (uint32_t)n[1], r;
        if ((int32_t)c < 1) r = 0;
        else {
            uint64_t s = (uint64_t)rd<uint32_t>(pCtx, 0x1c4) * 0x6ac690c5ull + rd<uint32_t>(pCtx, 0x1c8);
            wr<uint64_t>(pCtx, 0x1c4, s);
            r = (c & (c - 1)) == 0 ? (c - 1) & (uint32_t)s : (uint32_t)((s & 0xffffffffull) % c);
        }
        n[2] = (int32_t)r;
        wr<uint32_t>(n, 12, rd<uint32_t>(pCtx, 0x1cc));
        wr<uint32_t>(pCtx, 0x1cc, (uint32_t)(uintptr_t)n);
    }
found:
    n[2] = (n[2] + 1) % n[1];
    return n[2];   // EAX on return: the advanced cursor (0x42A70 takes it as the flag)
}
