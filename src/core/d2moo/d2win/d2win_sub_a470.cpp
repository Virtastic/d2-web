// src/core/d2win/d2win_sub_a470.cpp -- 1.13c D2Win+0x0A470 (ordinal 10165), transcribed from the cached decompile.
#include <cstdint>
#include <emscripten.h>
struct D2WinA470Env {
    void (*fog10116)();                           // Fog#10116, no args
    void (*strCopy)(char*, const char*, uint32_t); // Storm#503 (dst, src, 0x104)
    void (*strAppend)(char*, const char*);        // Storm#501 (second path attempt, as in the decompile)
    bool (*fileExists)(const char*);              // GetFileAttributesA != INVALID
    int (*fog10117)();
};
// 1.13c D2Win+0x0A470 -- true when d2char.mpq is found in neither the first nor the appended path and Fog#10117 returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE bool d2win_sub_a470(const D2WinA470Env* e) {
    char buf[0x104];
    e->fog10116();
    e->strCopy(buf, "d2char.mpq", 0x104);
    if (e->fileExists(buf)) return false;
    e->strAppend(buf, "d2char.mpq");
    if (e->fileExists(buf)) return false;
    return e->fog10117() == 0;
}
