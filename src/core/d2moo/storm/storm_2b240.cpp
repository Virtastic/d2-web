// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
// 1.13c Storm+0x2B240 SBigToBinaryBuffer (ordinal 638), stdcall (big, dst, cap, pCopied). The byte vector at +0x158
// {cap, count +0x15c, data +0x160, growth} is emptied (count = 0) and refilled by 0x2B020 (stack: &bytes, big; ret 8);
// n = min(count, cap) unsigned; n bytes from data to dst; *pCopied = n when pCopied is not null. Returns n.
static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
extern "C" uint32_t Storm13c_2B240(void (*encode2B020)(uint32_t bytes, uint32_t big), uint32_t big, uint32_t dst, uint32_t cap, uint32_t pCopied) {
    wr32(big + 0x15c, 0);
    encode2B020(big + 0x158, big);
    uint32_t n = rd32(big + 0x15c);
    if (n >= cap) n = cap;
    std::memcpy((void*)(uintptr_t)dst, (const void*)(uintptr_t)rd32(big + 0x160), n);
    if (pCopied) wr32(pCopied, n);
    return n;
}
