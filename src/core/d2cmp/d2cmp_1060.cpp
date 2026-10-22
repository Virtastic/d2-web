// src/core/d2cmp/d2cmp_1060.cpp -- 1.13c D2CMP.dll 0x1060, transcribed from the cached Ghidra decompile.
// Unlinks a two-dword node {link, ref} from an intrusive list, twice (compiler-inlined unwind/dtor body).
// Retail addresses are 32-bit; `mem` is the flat image they index (addr 0 = mem[0]).
#include <cstdint>
#include <cstring>

static uint32_t rd(uint8_t* m, uint32_t a) { uint32_t v; memcpy(&v, m + a, 4); return v; }
static void wr(uint8_t* m, uint32_t a, uint32_t v) { memcpy(m + a, &v, 4); }

// 1.13c D2CMP+0x1060 (160 bytes): __cdecl-ish, one pointer arg (node address).
// 1.13c D2CMP+0x1060
extern "C"
void d2cmp_1060(uint8_t* mem, uint32_t node) {
    for (int pass = 0; pass < 2; ++pass) {
        uint32_t p = rd(mem, node);
        if (p == 0) continue;
        uint32_t r = rd(mem, node + 4);
        uint32_t slot = ((int32_t)r < 0) ? ~r : node + (r - rd(mem, p + 4));
        wr(mem, slot, p);
        wr(mem, rd(mem, node) + 4, rd(mem, node + 4));
        wr(mem, node, 0);
        wr(mem, node + 4, 0);
    }
}
