// src/core/d2win/d2win_18af4.cpp -- 1.13c D2Win.dll+0x18AF4, MSVC CRT __ftol2 (x87 st0 -> int64, truncating), integer-only
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering and header layout only).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

// Retail truncates st0 toward zero into EDX:EAX. Here st0 arrives as the IEEE-754 double bits
// of the value (every caller in D2Win holds a double-rounded product; see d2win_ebe0.cpp). NaN,
// infinity and anything outside int64 give the x87 "integer indefinite" 0x8000000000000000.
// The game observes it through the progress bar's filled width (D2Win+0xEBE0).

// 1.13c D2Win+0x18AF4
extern "C" EMSCRIPTEN_KEEPALIVE int64_t d2win_ftol2_18AF4(uint64_t bits) {
    const uint64_t kIndefinite = 0x8000000000000000ull;
    uint32_t e = (uint32_t)(bits >> 52) & 0x7FF;
    uint64_t m = bits & 0xFFFFFFFFFFFFFull;
    bool neg = (bits >> 63) != 0;
    if (e == 0x7FF) return (int64_t)kIndefinite;
    if (e < 1023) return 0;                       // |v| < 1 (zero and denormals included)
    int32_t ue = (int32_t)e - 1023;
    if (ue >= 63) return (int64_t)kIndefinite;    // -2^63 itself also yields 0x8000000000000000
    m |= 1ull << 52;
    uint64_t v = ue >= 52 ? (m << (ue - 52)) : (m >> (52 - ue));
    return neg ? (int64_t)(0 - v) : (int64_t)v;
}
