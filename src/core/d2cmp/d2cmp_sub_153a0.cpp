// src/core/d2cmp/d2cmp_sub_153a0.cpp -- 1.13c D2CMP.dll 0x153A0 (export #10059, stdcall, ret 0x18).
// Retail layout: `indexes` is an array of 32-bit tile-index addresses, `out` a 32-bit slot array.
#include <cstdint>
#include <cstdlib>

// 0x12E60: EAX = index, EBX = key1c, stack (out, cap, key14, key18), ret 0x10; returns the count written.
using D2CMP153a0Query = uint32_t (*)(uint32_t index, uint32_t key1c, uint32_t* out, uint32_t cap,
                                     uint32_t key14, uint32_t key18);

// Query up to 32 tile indexes in order, packing matches into `out`; returns the count written
// (cap when a query fills the remaining room exactly).
// 1.13c D2CMP+0x153A0
extern "C" uint32_t d2cmp_sub_153a0(const uint32_t* indexes, uint32_t key14, uint32_t key18,
                                    uint32_t key1c, uint32_t* out, uint32_t cap,
                                    D2CMP153a0Query query) {
    if (!indexes) std::exit(-1);   // retail: Fog assert (string 0x5d) then _exit(-1)
    uint32_t used = 0, left = cap;
    for (int i = 0; i < 32 && indexes[i]; i++) {
        uint32_t n = query(indexes[i], key1c, out + used, left, key14, key18);
        left -= n; used += n;
        if (!left) return cap;
    }
    return cap - left;
}
