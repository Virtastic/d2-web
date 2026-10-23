// src/core/d2win/d2win_sub_17950.cpp -- 1.13c D2Win.dll control flag-0 setter, from the cached
// Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x17950.c).
#include <cstdint>

// 1.13c D2Win+0x17950
// c = control (dword [2] = flags); on != 0 sets gdwBitMasks[0] (=1), else clears it (gdwInvBitMasks[0]).
// Null control: no-op.
extern "C" void d2win_sub_17950(int32_t* c, int32_t on) {
    if (!c) return;
    if (on) c[2] |= 1u; else c[2] &= ~1u;
}
