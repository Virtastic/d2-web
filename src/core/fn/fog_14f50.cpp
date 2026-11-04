// probe for Fog 0x14F50 (sub_14F50, loaded module list via psapi): stdcall(hProcess = r[0]); blobs pn r[14] (the tuple's pins: [0] LoadLibraryA ok, [1] GetProcAddress ok,
// [2] Enum result, [3] bytes, [4] module count, 16 dwords per module from [8]: info/filename/basename results, base, size, path (4 dwords), name (4)), lg r[16] (hook log).
// Port: fog_sub_14f50 through its FogPsapiSeams. Each seam is modelled as the call retail makes: the four psapi calls are the hook on Fog+0x16920 (stdcall, 4 words, the
// out-values written through words 1..3), the logger is 0x190A0 (the error lines push a third word, the GetLastError value 5), the list free is 0x14C30 (a recorded no-register hook).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "hookenv.h"
struct FogPsapiSeams {
    bool (*load)();
    void (*unload)();
    int (*enumModules)(uint32_t proc, uint32_t* mods, uint32_t cb, uint32_t* needed);
    int (*modInfo)(uint32_t proc, uint32_t mod, uint32_t* info3, uint32_t cb);
    int (*fileName)(uint32_t proc, uint32_t mod, char* buf, uint32_t n);
    int (*baseName)(uint32_t proc, uint32_t mod, char* buf, uint32_t n);
    void* (*alloc)(uint32_t size);
    void (*log)(const char* fmt, ...);
    void (*freeList)(void* head);
};
extern FogPsapiSeams g_fogPsapi;
struct FogModNode;
extern "C" FogModNode* fog_sub_14f50(uint32_t proc);
namespace {
HookEnv* H;
const uint32_t* gPn;
uint32_t gMod;     // the module index the next three psapi calls serve
const uint32_t kLog = 0xB20268ECu;
// one psapi call = one h() of the shared stub, with a per-call table: (rva, ret), then (key, value) pairs for the out words
uint32_t stub(uint32_t ret, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t outWord, const uint32_t* v, uint32_t n) {
    uint32_t t[40]; uint32_t k = 0;
    t[k++] = 0x16920; t[k++] = ret;
    for (uint32_t i = 0; i < n; i++) { t[k++] = ((1u + outWord) << 24) | 0x16920; t[k++] = v[i]; }
    t[k++] = 0; t[k++] = 0;
    H->sc = t;
    return H->h(0x16920, 0, 0, 0, 0, 0, a0, a1, a2, a3);
}
bool load() {
    if (!gPn[0]) return false;
    if (!gPn[1]) {
        H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB2027678u);
        H->h(0x14C30, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        return false;
    }
    return true;
}
void unload() {}
int enumModules(uint32_t proc, uint32_t* mods, uint32_t cb, uint32_t* needed) {
    uint32_t h[256]; const uint32_t nm = gPn[4];
    for (uint32_t i = 0; i < (nm ? nm : 1); i++) h[i] = 0x100 + i;
    const int ret = (int)stub(gPn[2], proc, (uint32_t)(uintptr_t)mods, cb, (uint32_t)(uintptr_t)needed, 1, h, nm ? nm : 1);
    *needed = gPn[3];
    for (uint32_t i = 0; i < (nm ? nm : 1) && i < 256; i++) mods[i] = h[i];
    if (!ret) { H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB2027644u, 5); }
    return ret;
}
int modInfo(uint32_t proc, uint32_t mod, uint32_t* info, uint32_t cb) {
    const uint32_t* m = gPn + 8 + 16 * gMod;
    const uint32_t v[3] = { m[3], m[4], 0 };
    const int ret = (int)stub(m[0], proc, mod, (uint32_t)(uintptr_t)info, cb, 2, v, 3);
    info[0] = v[0]; info[1] = v[1]; info[2] = v[2];
    if (!ret) H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB20275C8u, 5);
    return ret;
}
int fileName(uint32_t proc, uint32_t mod, char* buf, uint32_t n) {
    const uint32_t* m = gPn + 8 + 16 * gMod;
    const int ret = (int)stub(m[1], proc, mod, (uint32_t)(uintptr_t)buf, n, 2, m + 5, 4);
    std::memcpy(buf, m + 5, 16);
    if (!ret) H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB20275A0u, 5);
    return ret;
}
int baseName(uint32_t proc, uint32_t mod, char* buf, uint32_t n) {
    const uint32_t* m = gPn + 8 + 16 * gMod;
    const int ret = (int)stub(m[2], proc, mod, (uint32_t)(uintptr_t)buf, n, 2, m + 9, 4);
    std::memcpy(buf, m + 9, 16);
    if (!ret) H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB2027578u, 5);
    else gMod++;
    return ret;
}
void* alloc(uint32_t size) { return std::calloc(1, size); }
void logf(const char* fmt, ...) {
    // the port's messages by text: 0x275F0 "\nEnumerate modules..." 0x27608 invalid number; the error lines are logged by the seams above
    if (!std::strcmp(fmt, "\nEnumerate modules...")) H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB20275F0u);
    else if (std::strstr(fmt, "invalid number")) H->h(0x190A0, 0, 0, 0, 0, 0, kLog, 0xB2027608u);
}
void freeList(void* head) { H->h(0x14C30, 0, 0, 0, 0, 0, 0, 0, 0, 0); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_14f50(uint32_t* r) {
    gPn = (const uint32_t*)(uintptr_t)r[14];
    const uint32_t none[2] = { 0, 0 };
    HookEnv e((uint32_t*)(uintptr_t)r[16], none);
    H = &e; gMod = 0;
    g_fogPsapi = FogPsapiSeams{ load, unload, enumModules, modInfo, fileName, baseName, alloc, logf, freeList };
    FogModNode* head = fog_sub_14f50(r[0]);
    r[4] = head ? 0xB5000000u : 0;
}
