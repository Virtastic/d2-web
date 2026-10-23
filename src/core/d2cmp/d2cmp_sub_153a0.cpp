// src/core/d2cmp/d2cmp_sub_153a0.cpp -- 1.13c D2CMP.dll 0x153A0 (export #10059), from the cached
// Ghidra decompile (build-wasm/ghidra-d2cmp/decomp/0x153A0.c). Own file
#include <cstdint>
#include <cstdlib>
struct D2CMPTileIndex;
extern "C" uint32_t d2cmp_12e60(const D2CMPTileIndex*, uint32_t key1c, const void** out,
                                uint32_t cap, uint32_t key14, uint32_t key18);

// Query up to 32 tile indexes in order, packing matches into `out`; returns the count written.
// 1.13c D2CMP+0x153A0
extern "C" uint32_t d2cmp_sub_153a0(const D2CMPTileIndex* const* indexes, uint32_t key14,
                                    uint32_t key18, uint32_t key1c, const void** out,
                                    uint32_t cap) {
    if (!indexes) std::exit(-1);   // retail: Fog assert (string 0x5d) then _exit(-1)
    uint32_t used = 0, left = cap;
    for (int i = 0; i < 32 && indexes[i]; i++) {
        uint32_t n = d2cmp_12e60(indexes[i], key1c, out + used, left, key14, key18);
        left -= n; used += n;
        if (!left) return cap;
    }
    return cap - left;
}
