// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
// 1.13c Storm+0x2DD70 SBigMod (ordinal 621), stdcall (dst, a, b). dst's scratch stack (16 slots of 0x14 bytes at +0x14, used
// count at +0x154) lends the next free slot: n = [dst+0x154], [dst+0x154] = n+1, then 0x2D830 (divide, ret 0x14) with
// stack (quotient, remainder, a, b, dst+0x14) where the quotient goes to the scratch slot and the remainder to dst; then [dst+0x154] is
// decremented in memory (not restored from n). eax of the divide is the result.
static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
extern "C" uint32_t Storm13c_2DD70(uint32_t (*divmod2D830)(uint32_t quot, uint32_t rem, uint32_t a, uint32_t b, uint32_t scratch), uint32_t dst, uint32_t a, uint32_t b) {
    const uint32_t n = rd32(dst + 0x154);
    const uint32_t slot = dst + 0x14 + n * 0x14;
    wr32(dst + 0x154, n + 1);
    const uint32_t r = divmod2D830(slot, dst, a, b, dst + 0x14);
    wr32(dst + 0x154, rd32(dst + 0x154) - 1);
    return r;
}
