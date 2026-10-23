#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" [[noreturn]] void d2_mcp_startup_overflow(uint32_t message);

static void ucpy(uint8_t* d, const char16_t* s) { std::size_t n = 0; while (s[n]) n++; std::memcpy(d, s, (n + 1) * 2); }

// 1.13c D2Win+0x0B650 (src/core/d2win/d2win_sub_b650.cpp:d2win_sub_B650)
// Fill a widget-text record: four Unicode strings at +0xCC/+0x14C/+0x1CC/+0x1EC, dwords at +0x22C/+0x230.
// NULL record: fatal (Fog code 0x89D, _exit(-1)).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_B650(uint8_t* p, const char16_t* a, const char16_t* b, const char16_t* c,
                                         const char16_t* d, uint32_t v1, uint32_t v2) {
    if (!p) d2_mcp_startup_overflow(0x89d);
    ucpy(p + 0xcc, a); ucpy(p + 0x14c, b); ucpy(p + 0x1cc, c); ucpy(p + 0x1ec, d);
    std::memcpy(p + 0x22c, &v1, 4); std::memcpy(p + 0x230, &v2, 4);
}
