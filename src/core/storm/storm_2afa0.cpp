// Prior art used: none beyond the 1.13c decompile and disassembly.
#include <cstdint>
#include <cstring>
// 1.13c Storm+0x2AFA0 SBigNew (ordinal 624), stdcall (ppBig). SMemAlloc(0x168, SBig.CPP tag, 0x521, 0); a non-null block
// is constructed by 0x39A0 (ret 4, returns the object); *ppBig = the constructor's result, or 0 when the alloc failed.
// Returns the stored word. The SEH frame around the constructor only unwinds a throwing ctor (none in 1.13c).
static void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
extern "C" uint32_t Storm13c_2AFA0(uint32_t (*alloc1C580)(uint32_t size, uint32_t tag, uint32_t line, uint32_t flags),
                                   uint32_t (*construct39A0)(uint32_t obj), uint32_t tag, uint32_t ppBig) {
    const uint32_t m = alloc1C580(0x168, tag, 0x521, 0);
    const uint32_t r = m ? construct39A0(m) : 0;
    wr32(ppBig, r);
    return r;
}
