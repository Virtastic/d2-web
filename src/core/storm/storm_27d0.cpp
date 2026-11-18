// 1.13c Storm+0x27D0 -- delete a region chunk: stdcall (ret 4), one stack word = the chunk.
// Retail is 32-bit: every address below is a 32-bit word and every TSLink is {link, tag} (a negative tag is the
// complement of the slot that points at this link, a non-negative one a byte offset from the next link's tag).
// The chunk's own link at +0x10 is unlinked, then 0x2740 runs with EAX = the chunk: each 0x60-byte element of the
// array at +8 (count at +4, re-read every pass) goes through 0x1C30 (SMemFree +0x40/+0x30/+0x20, then 0x1CF0 unlinks
// the element's links at +0xC and +4), and the array itself is freed when non-null.
// EAX at return is SMemFree's return for the array, or the count word when the array is null.
#include <cstdint>
#include <cstring>

extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);   // 0x1B960 SMemFree

namespace {
inline uint32_t rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void wr(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

// the shared unlink snippet: the link and tag words are re-read after the slot store, as retail does
void unlink27d0(uint32_t l) {
    uint32_t next = rd(l);
    if (next == 0) return;
    uint32_t tag = rd(l + 4);
    uint32_t slot = (int32_t)tag < 0 ? ~tag : tag + (l - rd(next + 4));
    wr(slot, next);
    uint32_t n2 = rd(l), t2 = rd(l + 4);
    wr(n2 + 4, t2);
    wr(l, 0);
    wr(l + 4, 0);
}

int32_t smemfree(uint32_t p, const char* tag) { return Storm_Sub1B960((void*)(uintptr_t)p, tag, -2); }

// 0x1C30: free the element's three named buffers, then 0x1CF0
void element1c30(uint32_t e) {
    if (uint32_t p = rd(e + 0x40)) smemfree(p, ".?AU_FOUNDPARAM@@");
    if (uint32_t p = rd(e + 0x30)) smemfree(p, ".?AURECTF@@");
    if (uint32_t p = rd(e + 0x20)) smemfree(p, ".?AU_SOURCE@@");
    unlink27d0(e + 0xC);
    unlink27d0(e + 4);
}

// 0x2740 (EAX = chunk)
uint32_t chunk2740(uint32_t c) {
    uint32_t eax = rd(c + 4);
    for (uint32_t i = 0; i < eax; i++) {
        element1c30(rd(c + 8) + i * 0x60);
        eax = rd(c + 4);
    }
    uint32_t data = rd(c + 8);
    if (data) eax = (uint32_t)smemfree(data, ".?AURGN@@");
    return eax;
}
}  // namespace

// 1.13c Storm+0x27D0
extern "C" uint32_t Storm_ChunkDelete_27D0(uint32_t c) {
    unlink27d0(c + 0x10);
    return chunk2740(c);
}
