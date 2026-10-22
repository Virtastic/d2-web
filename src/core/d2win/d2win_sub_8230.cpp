// src/core/d2win/d2win_sub_8230.cpp -- 1.13c D2Win+0x08230 (ordinal 10114), transcribed from the cached
// decompile. Control (32-bit layout): +0 type (4 = list), +0x48 head node, +0x5C flags, +0x60 selected index;
// node: +0 value, +0x1C next. Nodes are resolved through env->node (guest 32-bit pointer -> host bytes).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
struct D2WinSel8230Env {
    uint32_t (*errorText)(uint32_t);
    void (*reportError)(const char*, uint32_t);
    const uint8_t* (*node)(uint32_t);
};
static uint32_t rd(const uint8_t* p, int o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
// 1.13c D2Win+0x08230 -- value of the selected list node, 0 if none; null -> string 0x2A6, wrong type -> 0x2A7, then exit.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_8230(const uint8_t* p, const D2WinSel8230Env* e) {
    if (!p || rd(p, 0) != 4) {
        e->reportError("", e->errorText(p ? 0x2A7 : 0x2A6));
        std::_Exit(-1);
    }
    int32_t sel = (int32_t)rd(p, 0x60);
    if (sel == -1 || !(p[0x5C] & 1)) return 0;
    int32_t i = 0;
    for (uint32_t n = rd(p, 0x48); n; n = rd(e->node(n), 0x1C), i++)
        if (sel == i) return rd(e->node(n), 0);
    return 0;
}
