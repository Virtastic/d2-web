// src/core/d2win/d2win_85f0.cpp -- 1.13c D2Win+0x085F0 (ord 10035, 71 B), from the cached decompile.
// Alpha lane: implementation + marker only. Widget (type 4) -> its child at +0x90 (type 5) gets
// field +0x54 = value; returns 1. Any other input is fatal: Ordinal_10265(code) text, Ordinal_10024, _exit(-1).
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2Win85F0Env { void (*fatal)(uint32_t code); };  // 0x5A0 null, 0x5A1 not type 4, 0x7F child not type 5

// 1.13c D2Win+0x085F0
int32_t D2Win_85F0(const uint8_t* w, int32_t value, const D2Win85F0Env& env, uint8_t* (*deref)(uint32_t)) {
    uint32_t code;
    if (!w) code = 0x5A0;
    else {
        int32_t type; std::memcpy(&type, w, 4);
        if (type != 4) code = 0x5A1;
        else {
            uint32_t p; std::memcpy(&p, w + 0x90, 4);
            uint8_t* c = deref(p);
            int32_t ct; std::memcpy(&ct, c, 4);
            if (ct == 5) { std::memcpy(c + 0x54, &value, 4); return 1; }
            code = 0x7F;
        }
    }
    env.fatal(code);
    std::exit(-1);
}
