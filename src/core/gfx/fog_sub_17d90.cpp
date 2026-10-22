// Fog sub_17D90: __fastcall(LPCSTR path): DeleteFileA(path); result discarded.
#include <cstdio>
#include <emscripten.h>
// 1.13c Fog+0x17D90
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_17d90(const char* path) { std::remove(path); }
