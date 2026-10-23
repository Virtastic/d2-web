// src/core/storm/storm_sub_5040.cpp -- Storm.dll 0x5040 from build-wasm/ghidra-storm/decomp/0x5040.c.
// Prior art used: the decompile; sibling storm_sub_4430.cpp.
#include <cstdint>

void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);  // 0x1C580 (alloc)
uint32_t* Storm_Sub51F0(uint32_t* p);                                                // 0x51F0 (ctor)
void Storm_Sub4390(void* a, uint32_t* obj, int32_t src);                              // 0x4390

// 1.13c Storm+0x5040
// Allocates param_2+0x214 bytes tagged "__AUCONNREC__" (line -2, flags|8), runs the 0x51F0
// constructor on it, then (edi = src, register arg) runs 0x4390 on the result when src != 0.
extern "C"
uint32_t* Storm_Sub5040(uint32_t, int32_t param_2, uint32_t param_3, int32_t edi_src) {
    uint32_t* p = (uint32_t*)Storm_Sub1C580((uint32_t)param_2 + 0x214, "__AUCONNREC__", -2, param_3 | 8);
    uint32_t* r = p ? Storm_Sub51F0(p) : nullptr;
    if (edi_src != 0) Storm_Sub4390(nullptr, r, edi_src);
    return r;
}
