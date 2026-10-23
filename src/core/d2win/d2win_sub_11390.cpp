// src/core/d2win/d2win_sub_11390.cpp -- 1.13c D2Win+0x11390, transcribed from the cached decompile.
// p = the object *param_1 (32-bit layout): +0 type (9), +8 flags, +0x34 callback(param_1). bitMasks = gdwBitMasks.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
struct D2WinSel11390Env {
    uint32_t (*errorText)(uint32_t);
    void (*reportError)(const char*, uint32_t);
    uint32_t (*callback)(uint32_t cb, uint32_t param1);
};
static uint32_t rd(const uint8_t* p, int o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
// 1.13c D2Win+0x11390 -- wrong type: string 0xBE then exit; else if bit0 and bit2 of flags set and callback non-null, its result; else 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_11390(const uint8_t* p, uint32_t param1, const uint32_t* bitMasks, const D2WinSel11390Env* e) {
    if (rd(p, 0) != 9) {
        e->reportError("", e->errorText(0xBE));
        std::_Exit(-1);
    }
    uint32_t f = rd(p, 8), cb = rd(p, 0x34);
    if ((bitMasks[2] & f) && (bitMasks[0] & f) && cb) return e->callback(cb, param1);
    return 0;
}
