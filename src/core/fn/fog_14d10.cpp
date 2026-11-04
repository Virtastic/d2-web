// probe for Fog 0x14D10 (fold the Crashdump file): regs ecx = out table (r[0]), edx = in/out byte count (r[1]); blobs out r[14], len r[16],
// cd r[18] (dword 0 = file size, then the file bytes: what retail's fread hook feeds the callee), buf r[20], lg r[22], exe r[24], pin r[26]
// (dword 0 = GetFileAttributesA result, dword 1 = fopen result: the descriptor's pins, which no other blob carries).
// The port is fog_crashdump_fold with a host env that logs retail's hook records: 0x17C40 (+ its block), 0x2593 (+ block), 0x2B14, 0x2A44, 0x2B14,
// the allocator 0x1B560 (size, Exceptions.cpp, line 0x986), 0x268F (the file bytes written through stack word 0), 0x2C0B. The exe-dir lookup,
// GetFileAttributesA and DeleteFileA are pinned imports, not logged.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
struct FogCrashEnv {
    int (*exeDir)(char* out);
    void (*mkdirs)(char* path);
    uint32_t (*attrs)(const char* path);
    void* (*fopen)(const char* path);
    int (*fseekEnd)(void* f);
    long (*ftell)(void* f);
    void (*fseekSet)(void* f);
    void* (*alloc)(uint32_t n);
    void (*fread)(void* dst, uint32_t n, void* f);
    void (*fclose)(void* f);
    void (*del)(const char* path);
    void (*release)(void* p);
};
int fog_crashdump_fold(uint8_t* out, uint32_t* lenInOut, const FogCrashEnv* env);
namespace {
HookEnv* H;
const uint32_t* gPin;
uint32_t gBuf, gSize;
// the first `limit` bytes of the string's storage, zero-padded to 4 dwords (retail's hookblk BYTES)
void blk4(uint32_t rva, const char* s, uint32_t limit, uint32_t ndw) {
    uint32_t b[4] = {}; uint8_t* d = (uint8_t*)b; const uint32_t l = (uint32_t)strlen(s) + 1;
    memcpy(d, s, l < limit ? l : limit);
    H->blk(rva, b, ndw);
}
int exeDir(char* out) { std::strcpy(out, "C:\\d2\\"); return 1; }
void mk(char* p) { H->h(0x17C40, 0, (uint32_t)(uintptr_t)p); blk4(0x17C40, p, 7, 2); }
uint32_t attrs(const char*) { return gPin[0]; }
void* fo(const char* p) {
    const uint32_t f = H->h(0x2593, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)p, 0xB2027150u);
    blk4(0x2593, p, 12, 3);
    return (void*)(uintptr_t)f;
}
int seekEnd(void* f) { return (int)H->h(0x2B14, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)f, 0, 2); }
long tell(void* f) { return (long)H->h(0x2A44, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)f); }
void seekSet(void* f) { H->h(0x2B14, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)f, 0, 0); }
void* al(uint32_t n) { return (void*)(uintptr_t)H->h(0x1B560, n, 0, 0, 0, 0, 0xB20277A0u, 0x986); }
void rd(void* d, uint32_t n, void* f) { H->h(0x268F, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)d, 1, n, (uint32_t)(uintptr_t)f); }
void cl(void* f) { H->h(0x2C0B, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)f); }
void dl(const char*) {}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_14d10(uint32_t* r) {
    const uint32_t* cd = (const uint32_t*)(uintptr_t)r[18];
    gPin = (const uint32_t*)(uintptr_t)r[26];
    gBuf = r[20]; gSize = cd[0];
    uint32_t sc[2 * 48 + 2]; uint32_t k = 0;
    sc[k++] = 0x2593; sc[k++] = gPin[1];
    sc[k++] = 0x2A44; sc[k++] = gSize;
    sc[k++] = 0x1B560; sc[k++] = gBuf;
    for (uint32_t i = 0; i < gSize / 4 && i < 48; i++) { sc[k++] = (1u << 24) | 0x268F; sc[k++] = cd[1 + i]; }
    sc[k++] = 0; sc[k++] = 0;
    HookEnv e((uint32_t*)(uintptr_t)r[22], sc);
    H = &e;
    const FogCrashEnv env = { exeDir, mk, attrs, fo, seekEnd, tell, seekSet, al, rd, cl, dl, nullptr };
    r[4] = (uint32_t)fog_crashdump_fold((uint8_t*)(uintptr_t)r[0], (uint32_t*)(uintptr_t)r[1], &env);
}
