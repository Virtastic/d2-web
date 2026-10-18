// src/core/realm/bnclient_batch_4e0b.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x4E0B.c (image base 0x6FF20000).
//
// Row: ___sbh_alloc_new_group 0x4E0B, MSVC 2003 (VC7.1) Release CRT small-block heap. Same rule as
// D2MCPClient 0x393E (src/core/realm/mcp_sbh_alloc.cpp); VirtualAlloc is a seam.
#include <cstdint>
#include <emscripten.h>

struct BnSbhNewGroupEnv {
    uint32_t (*virtual_alloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect);
};

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// 1.13c BNClient+0x04E0B (262 B, caller 0x51F0 ___sbh_alloc_block): ___sbh_alloc_new_group(region).
// Pick the first zero bit of the committed-subregion mask (consumed most-significant first), initialise
// the 0x3F bucket lists of that group's descriptor array, and commit the 0x8000-byte group
// (MEM_COMMIT, PAGE_READWRITE). Seed each of the eight 0x1000-byte subregions with one free 0xFF0-byte
// block, its head/tail size words, and the links that chain the subregions; the outer ends link to
// bucket 0x3F. Finally clear the group's low-bucket bitmap, set its high-bucket bitmap to 1, bump the
// committed-group count (setting region[1] bit 0 on the first group) and clear the bit in region[2].
// Returns the group index, or -1 when the commit fails.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_sbh_alloc_new_group(uint32_t region, const BnSbhNewGroupEnv* env) {
    uint32_t control = P32(region + 0x10)[0];
    int32_t group = 0;
    uint32_t bits = P32(region + 8)[0];
    while ((int32_t)bits >= 0) { group++; bits <<= 1; }
    uint32_t buckets = (uint32_t)group * 0x204u + 0x144u + control;
    uint32_t it = buckets;
    for (int32_t n = 0x3F; n != 0; --n) {
        P32(it + 8)[0] = it;
        P32(it + 4)[0] = it;
        it += 8;
    }
    uint32_t lpAddress = (uint32_t)group * 0x8000u + P32(region + 0xC)[0];
    if (env->virtual_alloc(lpAddress, 0x8000, 0x1000, 4) == 0) return -1;
    if (lpAddress <= lpAddress + 0x7000u) {
        uint32_t p = lpAddress + 0x10;
        uint32_t count = ((lpAddress + 0x7000u - lpAddress) >> 12) + 1;
        do {
            P32(p - 8)[0] = 0xffffffffu;
            P32(p + 0xFEC)[0] = 0xffffffffu;
            P32(p)[0] = p + 0xFFC;
            P32(p - 4)[0] = 0xFF0;
            P32(p + 4)[0] = p - 0x1004;
            P32(p + 0xFE8)[0] = 0xFF0;
            p += 0x1000;
            count--;
        } while (count != 0);
    }
    P32(buckets + 0x1FC)[0] = lpAddress + 0xC;
    P32(lpAddress + 0x14)[0] = buckets + 0x1F8;
    P32(buckets + 0x200)[0] = lpAddress + 0x700C;
    P32(lpAddress + 0x7010)[0] = buckets + 0x1F8;
    P32(control + 0x44 + (uint32_t)group * 4)[0] = 0;
    P32(control + 0xC4 + (uint32_t)group * 4)[0] = 1;
    uint8_t committed = *P8(control + 0x43);
    *P8(control + 0x43) = (uint8_t)(committed + 1);
    if (committed == 0) P32(region + 4)[0] |= 1;
    P32(region + 8)[0] &= ~(0x80000000u >> ((uint8_t)group & 31));
    return group;
}
