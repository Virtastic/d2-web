// src/core/d2win/d2win_8170.cpp -- 1.13c D2Win+0x08170 (ord 10033, 180 B), from the cached decompile.
#include <cstdint>
#include <cstring>
#include <cstdlib>

// 1.13c D2Win+0x08170
uint32_t D2Win_8170(const uint8_t* w, int32_t n, int32_t slot, const uint8_t* (*deref)(uint32_t), void (*fatal)(uint32_t code)) {
    auto rd = [](const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; };
    uint32_t code;
    if (!w) code = 0x2C2;
    else if ((int32_t)rd(w) != 4) code = 0x2C3;
    else if (n > (int32_t)rd(w + 0x54)) code = 0x2C4;
    else if (slot > 4) code = 0x2C5;
    else {
        uint32_t node = rd(w + 0x48);
        int32_t i = 0;
        if (n > 0) {
            do {
                uint32_t nx = rd(deref(node) + 0x1C);
                if (!nx) break;
                i++; node = nx;
            } while (i < n);
        }
        if (!node) code = 0x2CF;
        else if (i == n) return rd(deref(node) + slot * 4);
        else code = 0x2D0;
    }
    fatal(code);
    std::exit(-1);
}
