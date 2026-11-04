// D2Launch 1.13c +0xF190: replay probe over d2launch_n31::FUN_6fa4f190 (src/core/d2launch/d2launch_n31.cpp).
// regs ecx edx: r[0] = out (0x22 bytes), r[1] = name. Blobs: lg r[14] hook log; out r[16]; nm r[18]; cd r[20] the pinned 11 default-name bytes (DAT 24d9c);
// fb r[22] the file bytes the fread recorder hands back; sc r[24] scripted (rva, value) answers.
// Seams: Fog 10115 (0x97CC) fills "C:\d2save\"; CRT fopen (0x16E8), fread (0x17E4) and fclose (0x18B5) are recorders (the path is logged as hookblk words).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include "hookenv.h"

namespace d2launch_n31 {
struct F190Seams {
    void (*dir)(char* buf);
    void* (*open)(const char* path);
    size_t (*read)(void* h, uint8_t* buf, size_t n);
    void (*close)(void* h);
    void (*fatal)(int);
};
int FUN_6fa4f190(uint8_t* out, const char* name, const uint8_t* classDefault, const F190Seams& S);
}

static HookEnv* he;
static const uint8_t* fb;

static void dir(char* buf) { he->h(0x97cc, 0, (uint32_t)(uintptr_t)buf, 0x400); memcpy(buf, "C:\\d2save\\", 11); }
static void* open_(const char* path) {
    uint32_t r = he->h(0x16e8, 0, 0, 0, 0, 0, he->stk(path), 0xB201ea00u);
    uint32_t len = (uint32_t)strlen(path) + 1, n = (len + 3) / 4;
    uint32_t w[130] = {}; memcpy(w, path, len);
    he->blk(0x16e8, w, n);
    return (void*)(uintptr_t)r;
}
static size_t read_(void* h, uint8_t* buf, size_t n) {
    uint32_t r = he->h(0x17e4, 0, 0, 0, 0, 0, he->stk(buf), 1, (uint32_t)n, (uint32_t)(uintptr_t)h);
    if (r) memcpy(buf, fb, r > 0x220 ? 0x220 : r);
    return r;
}
static void close_(void* h) { he->h(0x18b5, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)h); }
static void fatal(int) { abort(); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_f190(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[14], (const uint32_t*)(uintptr_t)r[24]); he = &e;
    fb = (const uint8_t*)(uintptr_t)r[22];
    static const d2launch_n31::F190Seams S = { dir, open_, read_, close_, fatal };
    r[4] = d2launch_n31::FUN_6fa4f190((uint8_t*)(uintptr_t)r[16], (const char*)(uintptr_t)r[18],
                                      (const uint8_t*)(uintptr_t)r[20], S);
}
