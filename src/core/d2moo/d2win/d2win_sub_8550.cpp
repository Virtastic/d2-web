// src/core/d2win/d2win_sub_8550.cpp -- 1.13c D2Win+0x08550 (ordinal 10117), transcribed from the cached
// decompile. Control (32-bit layout): +0 type (4), +0x90 sub-control pointer; sub: +0 type (5), +0x50 value.
// Sub pointer is resolved through env->node (guest 32-bit pointer -> writable host bytes).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
struct D2WinSet8550Env {
    uint32_t (*errorText)(uint32_t);
    void (*reportError)(const char*, uint32_t);
    uint8_t* (*node)(uint32_t);
};
static uint32_t rd(const uint8_t* p, int o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
// 1.13c D2Win+0x08550 -- store value in the sub-control; null -> string 0x5B4, wrong type -> 0x5B5, bad sub type -> 0xCA, then exit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_8550(const uint8_t* p, uint32_t value, const D2WinSet8550Env* e) {
    uint32_t msg = 0x5B4;
    if (p) {
        if (rd(p, 0) == 4) {
            uint8_t* s = e->node(rd(p, 0x90));
            if (rd(s, 0) == 5) { std::memcpy(s + 0x50, &value, 4); return; }
            msg = 0xCA;
        } else msg = 0x5B5;
    }
    e->reportError("", e->errorText(msg));
    std::_Exit(-1);
}
