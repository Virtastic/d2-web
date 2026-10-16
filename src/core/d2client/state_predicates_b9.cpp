// Pure predicates over D2Client globals; each global is a parameter (the caller reads the live global).
#include <emscripten.h>
#include <cstdint>

// 1.13c D2Client+0x42030: a = [0x11C3CC], b = [0x119898], c = [0x11988C]. The retail != tests are implied by the == tests.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_Pred42030(uint32_t a, uint32_t b, uint32_t c) { return a != 0 && b == 2 && c == 5; }

// 1.13c D2Client+0x42060: same globals as 0x42030.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_Pred42060(uint32_t a, uint32_t b, uint32_t c) { return a != 0 && (b == 1 || (b == 2 && (c == 4 || c == 5))); }

// 1.13c D2Client+0x427C0: e8 = [0x11C3E8], ec = [0x11C3EC], f0 = [0x11C3F0].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_Flags427C0(uint32_t e8, uint32_t ec, uint32_t f0) { return ((e8 || ec) ? 8u : 0u) | (f0 ? 4u : 0u); }

// 1.13c D2Client+0x45980: v = the eight dwords at [0x119757 + 4k].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_AnyNonzero45980(const uint32_t* v) { for (int i = 0; i < 8; ++i) if (v[i]) return 1; return 0; }

// 1.13c D2Client+0x8B760: s = [0x11BC34].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_State1to3_8B760(uint32_t s) { return s == 1 || s == 2 || s == 3; }
