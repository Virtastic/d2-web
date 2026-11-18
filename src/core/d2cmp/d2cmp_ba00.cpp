// src/core/d2cmp/d2cmp_ba00.cpp -- 1.13c D2CMP sub_BA00 (export 10044), transcribed from the cached decompile.
#include <cstdint>

struct D2CmpBa00Env {
    void (*b930)(uint32_t* a1);   // RVA 0xB930, one stack word (callee pops it)
};

// stdcall(a1, a2) ret 8: *a2 := 0x6C327, then 0xB930(a1).
// 1.13c D2CMP+0x0BA00
void d2cmp_ba00(uint32_t* a1, uint32_t* a2, const D2CmpBa00Env* e) {
    *a2 = 0x6C327;
    e->b930(a1);
}
