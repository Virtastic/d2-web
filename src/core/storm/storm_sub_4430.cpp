// src/core/storm/storm_sub_4430.cpp -- Storm.dll 0x4430 from build-wasm/ghidra-storm/decomp/0x4430.c.
// Prior art used: the decompile; sibling seams in storm_batch_tiny_1018.cpp.
#include <cstdint>

extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);  // 0x1C580 (alloc)
extern "C" void Storm_ListLink4390(uint32_t* eax, uint8_t* this_, uint8_t* node_arg, int32_t mode);   // 0x4390 (storm_batch_1000.cpp)

// 1.13c Storm+0x4430
// Allocates param_2+0x40 bytes tagged "__AUCMDDEF__" (line -2, flags|8), zeroes the first two
// dwords, then (edi = src, register arg) runs 0x4390 on it when src != 0. Returns the block.
extern "C"
uint32_t* Storm_Sub4430(uint32_t own, int32_t param_2, uint32_t param_3, int32_t edi_src) {
    uint32_t* p = (uint32_t*)Storm_Sub1C580((uint32_t)param_2 + 0x40, "__AUCMDDEF__", -2, param_3 | 8);
    if (p) { p[0] = 0; p[1] = 0; }
    if (edi_src != 0) Storm_ListLink4390((uint32_t*)(uintptr_t)own, nullptr, (uint8_t*)p, edi_src);   // EAX = own, ECX = 0
    return p;
}
