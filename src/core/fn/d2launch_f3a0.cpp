// D2Launch 1.13c +0xF3A0: replay probe over d2launch_sub_F3A0 (src/core/d2launch/d2launch_batch_n48.cpp).
// regs eax ecx: r[0] = EAX sub-directory (0 = none), r[1] = ECX name. Blobs: lg r[14] hook log; nm r[16] name; sb r[18] sub; sc r[20] scripted (rva, value) answers; fd r[22] scripted find names (48 bytes: first, next).
// Seams: Fog 10115 (0x97CC) writes "C:\d2save\", Fog 10114 (0x9832) is a recorder; the kernel32 imports (IAT slots 0x1C35C FindFirst, 0x1C34C FindNext, 0x1C350 GetFileAttributes, 0x1C348 DeleteFile) are recorders;
// every pattern/path string is logged as hookblk words (the string and its NUL, rest zero).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"

typedef uintptr_t u;
struct LaunchHost { u (*ord)(uint32_t, u, u, u); void* (*data)(uint32_t); void (*exit_)(int); };
struct LaunchFind {
    void* (*first)(const char*, char*);
    bool (*next)(void*, char*);
    void (*close)(void*);
    void (*stamp)(void*, uint32_t[2]);
};
extern "C" void d2launch_sub_F3A0(const LaunchHost&, const LaunchFind&, const char*, const char*, bool (*)(const char*), bool (*)(const char*));

static HookEnv* he;
static const uint8_t* fd;

static void blkStr(uint32_t rva, const char* s) {
    uint32_t len = (uint32_t)strlen(s) + 1, n = (len + 3) / 4;
    uint32_t w[130] = {};
    memcpy(w, s, len);
    he->blk(rva, w, n);
}
static u ord(uint32_t o, u a, u b, u c) {
    if (o == 10115) { he->h(0x97cc, 0, (uint32_t)a, 0x104); memcpy((void*)a, "C:\\d2save\\", 11); return 0; }
    if (o == 10114) { he->h(0x9832, 0, (uint32_t)a); return 0; }
    if (o == 501) { strncpy((char*)a, (const char*)b, c); ((char*)a)[c - 1] = 0; return 0; }
    abort();
}
static void* first(const char* pat, char* out) {
    uint32_t r = he->h(0x1c35c, 0, 0, 0, 0, 0, he->stk(pat), 0xB3000000u);
    blkStr(0x1c35c, pat);
    strncpy(out, (const char*)fd, 40); out[39] = 0;
    if (r == 0xFFFFFFFFu) return nullptr;
    return (void*)(uintptr_t)r;
}
static bool next(void* h, char* out) {
    uint32_t r = he->h(0x1c34c, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)h, 0xB3000000u);
    strncpy(out, (const char*)fd + 48, 40); out[39] = 0;
    return r != 0;
}
static bool isDir(const char* p) { uint32_t a = he->h(0x1c350, 0, 0, 0, 0, 0, he->stk(p)); blkStr(0x1c350, p); return (a & 0x10) != 0; }
static bool del(const char* p) { uint32_t a = he->h(0x1c348, 0, 0, 0, 0, 0, he->stk(p)); blkStr(0x1c348, p); return a != 0; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_f3a0(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[14], (const uint32_t*)(uintptr_t)r[20]); he = &e;
    fd = (const uint8_t*)(uintptr_t)r[22];
    static const LaunchHost H = { ord, nullptr, nullptr };
    static const LaunchFind F = { first, next, nullptr, nullptr };
    d2launch_sub_F3A0(H, F, (const char*)(uintptr_t)r[1], (const char*)(uintptr_t)r[0], isDir, del);
    r[4] = 0;
}
