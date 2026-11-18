// src/core/d2win/d2win_8690.cpp -- 1.13c D2Win+0x08690 (ord 10089, 66 B), from the cached decompile.
// Widget (type 4) -> its child at +0x90 (type 5): returns field +0x4C. Any other input is fatal:
// Ordinal_10265(code) text, Ordinal_10024, _exit(-1).
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2Win8690Env { void (*fatal)(uint32_t code); };  // 0x58C null, 0x58D not type 4, 0x74 child not type 5

// 1.13c D2Win+0x08690
int32_t D2Win_8690(const uint8_t* w, const D2Win8690Env& env, uint8_t* (*deref)(uint32_t)) {
    uint32_t code;
    if (!w) code = 0x58C;
    else {
        int32_t type; std::memcpy(&type, w, 4);
        if (type != 4) code = 0x58D;
        else {
            uint32_t p; std::memcpy(&p, w + 0x90, 4);
            uint8_t* c = deref(p);
            int32_t ct; std::memcpy(&ct, c, 4);
            if (ct == 5) { int32_t v; std::memcpy(&v, c + 0x4C, 4); return v; }
            code = 0x74;
        }
    }
    env.fatal(code);
    std::exit(-1);
}
