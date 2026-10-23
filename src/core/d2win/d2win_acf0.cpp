// src/core/d2win/d2win_acf0.cpp -- 1.13c D2Win+0x0ACF0 (ord 10163, 52 B), from the cached decompile.
// Type-5 widget: +0x48 = max(value,0); returns 1. Other type is fatal (error 0x56).
#include <cstdint>
#include <cstring>
#include <cstdlib>

// 1.13c D2Win+0x0ACF5
int32_t D2Win_ACF0(uint8_t* w, int32_t value, void (*fatal)(uint32_t code)) {
    int32_t type; std::memcpy(&type, w, 4);
    if (type != 5) { fatal(0x56); std::exit(-1); }
    int32_t v = value < 1 ? 0 : value;
    std::memcpy(w + 0x48, &v, 4);
    return 1;
}
