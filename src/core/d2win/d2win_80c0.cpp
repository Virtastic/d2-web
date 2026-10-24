// src/core/d2win/d2win_80c0.cpp -- 1.13c D2Win+0x080C0 (ord 10015, 74 B), from the cached decompile.
#include <cstdint>
#include <cstring>
#include <cstdlib>

// 1.13c D2Win+0x080C0
// Store value in a type-4 control's slot array: count at +0x64, slots from +0x68. Fatal = host seam, then _exit(-1).
void D2Win_80C0(uint8_t* w, int32_t idx, int32_t value, void (*fatal)(uint32_t code)) {
    uint32_t code;
    int32_t type, count;
    if (!w) code = 0x2EF;
    else if ((std::memcpy(&type, w, 4), type != 4)) code = 0x2F0;
    else if ((std::memcpy(&count, w + 0x64, 4), idx < 0 || idx >= count)) code = 0x2F2;
    else { std::memcpy(w + 0x68 + idx * 4, &value, 4); return; }
    fatal(code);
    std::exit(-1);
}
