// src/core/storm/storm_1d460.cpp -- 1.13c Storm.dll Storm+0x1D460, transcribed from the cached decompile
// (build-wasm/ghidra-storm/decomp/0x1D460.c). Removes every AUTIMERREC whose (a, b) pair matches and
// returns 1 when any matched. Retail globals are read at their retail addresses, as in storm_batch_b51.cpp.
#include <cstdint>
#include "storm_retail_globals.h"

extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);  // 0x1B960 SMemFree
void Storm_Sub4D60(uint8_t* node);                                          // 0x4D60 list unlink (storm_batch_b47.cpp)

namespace {
uint32_t& w(uint32_t a) { return Storm_IsRetailGlobal(a) ? Storm_RetailW(a) : *(uint32_t*)(uintptr_t)a; }
uint32_t& u32(uint32_t p, uintptr_t off) { return *(uint32_t*)(uintptr_t)(p + off); }
}  // namespace

// 1.13c Storm+0x1D460
uint32_t Storm_Sub1D460(int32_t a, int32_t b) {
    uint32_t found = 0;
    uint32_t node = (int32_t)w(0x6FC459FC) < 1 ? 0 : w(0x6FC459FC);
    while ((int32_t)node > 0) {
        if (u32(node, 8) == (uint32_t)a && u32(node, 0xc) == (uint32_t)b) {
            const uint32_t next = u32(node, 4);
            found = 1;
            Storm_Sub4D60((uint8_t*)(uintptr_t)node);
            (void)Storm_Sub1B960((void*)(uintptr_t)node, "__AUTIMERREC__", -2);
            node = (int32_t)next < 1 ? 0 : next;
        } else {
            node = u32(node, 4);
        }
    }
    return found;
}
