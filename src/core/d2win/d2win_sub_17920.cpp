// src/core/d2win/d2win_sub_17920.cpp -- 1.13c D2Win+0x17920 (ordinal 10108), transcribed from the cached
// decompile. Seams: Fog #10265/#10024 (error text/report); gdwBitMasks[0] read through the env.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
struct D2WinBitEnv {
    uint32_t (*errorText)(uint32_t);
    void (*reportError)(const char*, uint32_t);
    uint32_t bitMask0; // *gdwBitMasks
};
// 1.13c D2Win+0x17920 -- return (*(p+8) & gdwBitMasks[0]); null p reports string 0x32E and exits.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_17920(const uint8_t* p, const D2WinBitEnv* e) {
    if (!p) {
        e->reportError("", e->errorText(0x32E));
        std::_Exit(-1);
    }
    uint32_t v;
    std::memcpy(&v, p + 8, 4);
    return v & e->bitMask0;
}
